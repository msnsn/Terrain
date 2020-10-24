#pragma once

#include <vector>

class Root;
class GLFWwindow;
class GameObject;

class Vector3;

class Renderer
{
public:
    Renderer(Root* c_root, int* c_b_ret);
    virtual ~Renderer();

    void pre_render_steps();
    void render(GameObject* gm);
    void post_render_steps();

    void refresh_view();

    inline GLFWwindow* getWindow(){return window;}

    Vector3 getCameraPosition();
    Vector3 getCameraRotation();

    inline void setCameraPosition(float x, float y, float z)
    {
        cam_pos_x = x;
        cam_pos_y = y;
        cam_pos_z = z;
    }

    inline void setCameraRotation(float x, float y, float z)
    {
        cam_rot_x = x;
        cam_rot_y = y;
        cam_rot_z = z;
    }

protected:
    Root* root;
    int* b_ret; //Le code d'erreur que l'on retourne.


    GLFWwindow* window;

    float cam_pos_x, cam_pos_y, cam_pos_z;
    float cam_rot_x, cam_rot_y, cam_rot_z;

    static void glfw_window_resize_callback(GLFWwindow* wind, int w, int h);
};
