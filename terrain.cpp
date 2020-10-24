#include "terrain.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "vector3.hpp"
#include "vbo.hpp"
#include "renderer.hpp"
#include "root.hpp"




Terrain::Terrain(Root* c_root) :
    GameObject(c_root),
    chunks(std::vector<TerrainChunk*>()),
    queuedChunkGenerationData(std::vector<ChunkGenerationData*>()),
    chunkGenerationRequests(std::vector<ChunkGenerationRequest*>())

{
    noise.SetNoiseType(FastNoise::Simplex);


    chunkGenerationThread = boost::thread(&Terrain::processChunkGenerationRequests, this);

}

Terrain::~Terrain()
{
    chunkGenerationThread.interrupt();
}

void Terrain::update(float deltaTime)
{
    Vector3 camPos = root->getRenderer()->getCameraPosition();

    int xcc = std::floor(camPos.x / chunkLength);
    int ycc = std::floor(camPos.z / chunkLength);

    //Suppression de chunks lointains
    float maxDistance = 16;
    for (int i = 0; i < chunks.size(); ++i)
    {
        TerrainChunk* chnk = chunks[i];

        int xd = chnk->xCoord - xcc;
        int yd = chnk->yCoord - ycc;
        float dst = std::pow(std::pow(xd, 2)+std::pow(yd, 2), 0.5f);

        if (dst > maxDistance*2)
        {
            deleteChunk(chnk->xCoord, chnk->yCoord);
            --i;
        }
    }

    requestsMutex.lock();
    //Création de requêtes de génération de chunks
    for (int x = -maxDistance+1; x < maxDistance-1; ++x)
    {
        for (int y = -maxDistance+1; y < maxDistance-1; ++y)
        {
            float dst = std::pow(std::pow(x, 2)+std::pow(y, 2), 0.5f);
            if (getChunk(xcc+x, ycc+y) == nullptr && !hasChunkGenerationRequest(xcc+x, ycc+y) && dst < maxDistance)
                requestChunkGeneration(xcc+x, ycc+y);
        }
    }

    //Supression des requêtes trop lointaines
    for (int i = 0; i < chunkGenerationRequests.size(); ++i)
    {
        ChunkGenerationRequest* chnk = chunkGenerationRequests[i];
        int xd = chnk->xCoord - xcc;
        int yd = chnk->yCoord - ycc;
        float dst = std::pow(std::pow(xd, 2)+std::pow(yd, 2), 0.5f);

        if (dst > maxDistance*2)
        {
            chunkGenerationRequests.erase(chunkGenerationRequests.begin()+i);
            delete chnk;
            --i;
        }
    }

    requestsMutex.unlock();

    stateMutex.lock();
    bool stateLocked = true;

    if (chunkGenerationRequests.size() > 0 && running == false)
    {
        stateMutex.unlock();
        stateLocked = false;
        chunkGenerationThread = boost::thread(&Terrain::processChunkGenerationRequests, this);
    }

    if (stateLocked)
        stateMutex.unlock();


    queueMutex.lock();
    processQueuedChunkData();
    queueMutex.unlock();
}

void Terrain::requestChunkGeneration(int xCoord, int yCoord)
{
    if (hasChunkGenerationRequest(xCoord, yCoord))
        return;

    ChunkGenerationRequest* req = new ChunkGenerationRequest();
    req->xCoord = xCoord;
    req->yCoord = yCoord;
    chunkGenerationRequests.push_back(req);
}

bool Terrain::hasChunkGenerationRequest(int xCoord, int yCoord)
{
    for (int i = 0; i < chunkGenerationRequests.size(); ++i)
    {
        if (chunkGenerationRequests[i]->xCoord == xCoord && chunkGenerationRequests[i]->yCoord == yCoord)
        {
            return true;
        }
    }
    return false;
}


void Terrain::processQueuedChunkData()
{
    while(queuedChunkGenerationData.size() > 0)
    {
        ChunkGenerationData* cdata = queuedChunkGenerationData[0];

        if (getChunk(cdata->xCoord, cdata->yCoord) != nullptr)
            deleteChunk(cdata->xCoord, cdata->yCoord);

        TerrainChunk* tchk = new TerrainChunk();
        tchk->xCoord = cdata->xCoord;
        tchk->yCoord = cdata->yCoord;

        GameObject* gm = new GameObject(root);
        tchk->gameObject = gm;

        gm->setPosition(getPosition().x + cdata->xCoord*chunkLength, getPosition().y, getPosition().z + cdata->yCoord*chunkLength);

        chunks.push_back(tchk);


        gm->createVBO("gl_Vertex", GL_ARRAY_BUFFER);
        gm->createVBO("gl_Color", GL_ARRAY_BUFFER);
        gm->getVBO("gl_Vertex")->buffer_data(cdata->vertices, tessellations*tessellations*36, GL_DYNAMIC_DRAW);
        gm->getVBO("gl_Color")->buffer_data(cdata->colors, tessellations*tessellations*36, GL_DYNAMIC_DRAW);

        gm->setShaderProgram(getShaderProgram());

        queuedChunkGenerationData.erase(queuedChunkGenerationData.begin());
        delete cdata;
    }
}

void Terrain::processChunkGenerationRequests()
{


    stateMutex.lock();
    running = true;
    stateMutex.unlock();

    requestsMutex.lock();
    bool requestLock = true;

    while (chunkGenerationRequests.size() > 0)
    {
        if (!requestLock)
            requestsMutex.lock();
        ChunkGenerationRequest* req = chunkGenerationRequests[0];
        requestsMutex.unlock();

        generateChunkData(req->xCoord, req->yCoord);

        requestsMutex.lock();
        chunkGenerationRequests.erase(chunkGenerationRequests.begin());
        delete req;
        requestsMutex.unlock();
        requestLock = false;
    }

    stateMutex.lock();
    running = false;
    stateMutex.unlock();
}


void Terrain::generateChunkData(int xCoord, int yCoord)
{


    ChunkGenerationData* cd = new ChunkGenerationData();
    cd->vertices = new float[tessellations*tessellations*36];//data;
    cd->colors = new float[tessellations*tessellations*36];//color;
    cd->xCoord = xCoord;
    cd->yCoord = yCoord;


    //Tessellation

    float tlength = chunkLength/(float)tessellations;

    int tndx = 0;
    for (int txc = 0; txc < tessellations; ++txc)
    {
        for (int txy = 0; txy < tessellations; ++txy)
        {
            cd->vertices[tndx] = txc*tlength;
            cd->vertices[tndx+1] = getHeight(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength));
            cd->vertices[tndx+2] = -1*txy*tlength;

            cd->vertices[tndx+3] = txc*tlength;
            cd->vertices[tndx+4] = getHeight(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength));
            cd->vertices[tndx+5] = -1*txy*tlength - tlength;

            cd->vertices[tndx+6] = txc*tlength + tlength;
            cd->vertices[tndx+7] = getHeight(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength));
            cd->vertices[tndx+8] = -1*txy*tlength - tlength;

            cd->vertices[tndx+9] = txc*tlength + tlength;
            cd->vertices[tndx+10] = getHeight(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength));
            cd->vertices[tndx+11] = -1*txy*tlength;

            cd->vertices[tndx+12] = txc*tlength;
            cd->vertices[tndx+13] = getHeight(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength));
            cd->vertices[tndx+14] = -1*txy*tlength;

            cd->vertices[tndx+15] = txc*tlength + tlength;
            cd->vertices[tndx+16] = getHeight(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength));
            cd->vertices[tndx+17] = -1*txy*tlength - tlength;




            cd->colors[tndx] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).x;
            cd->colors[tndx+1] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).y;
            cd->colors[tndx+2] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).z;

            cd->colors[tndx+3] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).x;
            cd->colors[tndx+4] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).y;
            cd->colors[tndx+5] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).z;

            cd->colors[tndx+6] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).x;
            cd->colors[tndx+7] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).y;
            cd->colors[tndx+8] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).z;

            cd->colors[tndx+9] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).x;
            cd->colors[tndx+10] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).y;
            cd->colors[tndx+11] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).z;

            cd->colors[tndx+12] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).x;
            cd->colors[tndx+13] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).y;
            cd->colors[tndx+14] = getColor(xCoord*chunkLength+txc*tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength)).z;

            cd->colors[tndx+15] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).x;
            cd->colors[tndx+16] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).y;
            cd->colors[tndx+17] = getColor(xCoord*chunkLength+txc*tlength + tlength, yCoord*chunkLength+chunkLength-(-1)*(txy*tlength + tlength)).z;

            //L'eau:

            cd->vertices[tndx+18] = txc*tlength;
            cd->vertices[tndx+19] = waterHeight;
            cd->vertices[tndx+20] = -1*txy*tlength;

            cd->vertices[tndx+21] = txc*tlength;
            cd->vertices[tndx+22] = waterHeight;
            cd->vertices[tndx+23] = -1*txy*tlength - tlength;

            cd->vertices[tndx+24] = txc*tlength + tlength;
            cd->vertices[tndx+25] = waterHeight;
            cd->vertices[tndx+26] = -1*txy*tlength - tlength;

            cd->vertices[tndx+27] = txc*tlength + tlength;
            cd->vertices[tndx+28] = waterHeight;
            cd->vertices[tndx+29] = -1*txy*tlength;

            cd->vertices[tndx+30] = txc*tlength;
            cd->vertices[tndx+31] = waterHeight;
            cd->vertices[tndx+32] = -1*txy*tlength;

            cd->vertices[tndx+33] = txc*tlength + tlength;
            cd->vertices[tndx+34] = waterHeight;
            cd->vertices[tndx+35] = -1*txy*tlength - tlength;



            for (int i = 0; i < 6; ++i)
            {
                cd->colors[tndx+18+i*3] = waterColor.x;
                cd->colors[tndx+18+i*3+1] = waterColor.y;
                cd->colors[tndx+18+i*3+2] = waterColor.z;
            };

            tndx += 36;


        }
    }


    queueMutex.lock();
    queuedChunkGenerationData.push_back(cd);
    queueMutex.unlock();
}


void Terrain::deleteChunk(int xCoord, int yCoord)
{
    TerrainChunk* chunk = getChunk(xCoord, yCoord);
    if (chunk != nullptr)
    {
        std::vector<TerrainChunk*>::iterator it;
        for (it = chunks.begin(); it != chunks.end(); ++it)
        {
            if ((*it) == chunk)
                {
                    chunks.erase(it);
                    break;
                }
        }
        delete chunk->gameObject;
        delete chunk;
    }
}


float Terrain::getHeight(float x_pos, float y_pos)
{
    float oct1 = noise.GetNoise(x_pos/20.f, y_pos/20.f)*100;
    float oct2 = noise.GetNoise(x_pos/4, y_pos/4)*6;

    return oct1+oct2;
}

Vector3 Terrain::getColor(float x_pos, float y_pos)
{
    float height = getHeight(x_pos, y_pos);
    if (height < 30)
        return Vector3(194, 178, 128)/255.f;
    if (height <= 2000)
        return Vector3(126, 200, 80)/255.f;
    else return Vector3(128, 132, 135)/255.f;
}

TerrainChunk* Terrain::getChunk(int xCoord, int yCoord)
{
    std::vector<TerrainChunk*>::iterator it;
    for (it = chunks.begin(); it != chunks.end(); ++it)
    {
        if ((*it)->xCoord == xCoord && (*it)->yCoord == yCoord)
            return (*it);
    }

    return nullptr;
}
