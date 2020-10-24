#pragma once

#include "gameobject.hpp"
#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#include "vector3.hpp"
#include "libs/FastNoise.h"

class VBO;

struct TerrainChunk
{
    GameObject* gameObject = nullptr;
    int xCoord, yCoord;
};

struct ChunkGenerationRequest
{
    int xCoord;
    int yCoord;
};

struct ChunkGenerationData
{
    int xCoord = 0;
    int yCoord = 0;

    float* vertices = nullptr;
    float* colors = nullptr;

    ~ChunkGenerationData()
    {
        delete[] vertices;
        delete[] colors;
    }
};

class Terrain : public GameObject
{
    public:
        Terrain(Root* c_root);
        virtual ~Terrain();

        void update(float deltaTime);

        void requestChunkGeneration(int xCoord, int yCoord);
        bool hasChunkGenerationRequest(int xCoord, int yCoord);
        void deleteChunk(int xCoord, int yCoord);

        TerrainChunk* getChunk(int xCoord, int yCoord);
        void processQueuedChunkData();

        float getHeight(float x_pos, float y_pos);
        Vector3 getColor(float x_pos, float y_pos);


        //Generation thread

        void processChunkGenerationRequests();
        void generateChunkData(int xCoord, int yCoord);


    protected:
        std::vector<TerrainChunk*> chunks;

        std::vector<ChunkGenerationRequest*> chunkGenerationRequests;
        std::vector<ChunkGenerationData*> queuedChunkGenerationData;

        boost::mutex requestsMutex, queueMutex, stateMutex;

        FastNoise noise;

        boost::thread chunkGenerationThread;
        bool running = false;

        Vector3 waterColor = Vector3(0, 105, 148)/255.f;

        float chunkLength = 126;
        float waterHeight = 0;
        int tessellations = 10;
};


