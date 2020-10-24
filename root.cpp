#include "root.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <math.h>

#include "renderer.hpp"
#include "vbo.hpp"
#include "vector3.hpp"

#include "gameobject.hpp"
#include "terrain.hpp"

#include "shader.hpp"
#include "program.hpp"

Root* Root::instance = nullptr;

Root::Root(int* c_b_ret) :
    b_ret(c_b_ret),
    gameObjects(std::vector<GameObject*>()),
    xppos(0),
    yppos(0),
    speed(1)

{
    init_procedure();
    end_procedure();

}

Root::~Root()
{

}




void Root::init_procedure()
{

    //Initialisation.

    Root::instance = this;

    if (!glfwInit())
    {
        *b_ret = 1;
        delete this;
        return;
    }



    renderer = new Renderer(this, b_ret);

    glfwSetInputMode(getRenderer()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(getRenderer()->getWindow(), Root::scroll_callback);

    //Begin

    /*GameObject* go = new GameObject(this);
    go->getVBO()->buffer_data({-1, 0, 0, 0, 0, 0, 0, 1, 0}, GL_STATIC_DRAW);
    go->setPosition(0, 0, 10);
    go->setRotation(0, 180, 0);*/


    Shader vertShader = Shader("shaders/vert.vert", GL_VERTEX_SHADER);
    Shader fragShader = Shader("shaders/frag.frag", GL_FRAGMENT_SHADER);

    Program prog = Program();
    prog.attachShader(vertShader);
    prog.attachShader(fragShader);
    prog.link();



    Terrain* t = new Terrain(this);
    t->setShaderProgram(prog);

    //Commencer la boucle principale.

    float deltaTime = 0;
    float _lCall = glfwGetTime();

    while (!glfwWindowShouldClose(renderer->getWindow()))
    {
        deltaTime = glfwGetTime()-_lCall;
        _lCall = glfwGetTime();
        loop_routine(deltaTime);
    }

    //prog.clearProgram();
    //vertShader.clearShader();
    //fragShader.clearShader();

}

void Root::end_procedure()
{
    delete renderer;

    glfwTerminate();
}

void Root::loop_routine(float deltaTime)
{
    camera_control(deltaTime);

    renderer->pre_render_steps();

    for (int i = 0; i < gameObjects.size(); ++i)
    {
        GameObject* gm = gameObjects[i];
        gm->update(deltaTime);
        renderer->render(gm);
    }

    renderer->post_render_steps();


    glfwPollEvents();
    glfwSwapBuffers(renderer->getWindow());

    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(getRenderer()->getWindow(), GLFW_TRUE);

}


void Root::camera_control(float deltaTime)
{
    float sensitivity = 10;

    double xpos, ypos;
    glfwGetCursorPos(getRenderer()->getWindow(), &xpos, &ypos);

    double xdpos=-(xpos-xppos), ydpos=-(ypos-yppos);

    Vector3 nRot = getRenderer()->getCameraRotation() + Vector3(ydpos*sensitivity*deltaTime, xdpos*sensitivity*deltaTime, 0);

    getRenderer()->setCameraRotation(nRot.x, nRot.y, 0);

    Vector3 camSpeed = Vector3();

    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
        camSpeed.z += 1;
    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
        camSpeed.z -= 1;
    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
        camSpeed.x += 1;
    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
        camSpeed.x -= 1;
    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
        camSpeed.y += 1;
    if (glfwGetKey(getRenderer()->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camSpeed.y -= 1;



    Vector3 radCRot = (Vector3(360,360,360)-getRenderer()->getCameraRotation()) / 360 * (2 * 3.14);

    Vector3 nPos = getRenderer()->getCameraPosition() + Vector3(std::sin(radCRot.y) * camSpeed.z * std::cos(radCRot.x) + std::cos(radCRot.y) * camSpeed.x,
                                                                -std::sin(radCRot.x) * camSpeed.z + camSpeed.y,
                                                                std::cos(radCRot.y) * camSpeed.z * std::cos(radCRot.x) + std::sin(radCRot.y) * -camSpeed.x) * exp(speed)/100.f;

    getRenderer()->setCameraPosition(nPos.x, nPos.y, nPos.z);

    xppos = xpos; yppos = ypos;
}

void Root::scroll_callback(GLFWwindow* win, double x, double y)
{
    Root::instance->speed += y;
}


void Root::registerGameObject(GameObject* gm)
{
    gameObjects.push_back(gm);
}
void Root::unregisterGameObject(GameObject* gm)
{
    std::vector<GameObject*>::iterator it;
    for (it = gameObjects.begin(); it != gameObjects.end(); ++it)
    {
        if ((*it) == gm)
        {
            gameObjects.erase(it);
            return;
        }
    }
}
