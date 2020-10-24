#pragma once

#include <vector>

class Renderer;
class Updater;
class GameObject;
class GLFWwindow;

class Root
{
public:
    Root(int* c_b_ret);
    virtual ~Root();

    inline Renderer* getRenderer()
    {
        return renderer;
    }

    static Root* instance;


    void registerGameObject(GameObject* gm);
    void unregisterGameObject(GameObject* gm);

    static void scroll_callback(GLFWwindow* win, double x, double y);

protected:
    int* b_ret; //Le code d'erreur que l'on retourne.
    double xppos, yppos, speed;

    Renderer* renderer;

    void init_procedure();
    void end_procedure();

    void loop_routine(float deltaTime);

    void camera_control(float deltaTime);



    std::vector<GameObject*> gameObjects;

};


