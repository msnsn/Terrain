#include "renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "root.hpp"
#include "gameobject.hpp"
#include "vbo.hpp"

#include "vector3.hpp"

#include <iostream>

void Renderer::glfw_window_resize_callback(GLFWwindow* wind, int w, int h)
{
    Root::instance->getRenderer()->refresh_view();
}


Renderer::Renderer(Root* c_root, int* c_b_ret):
    root(c_root),
    b_ret(c_b_ret),
    window(nullptr),

    cam_pos_x(0), cam_pos_y(0), cam_pos_z(0),
    cam_rot_x(0), cam_rot_y(0), cam_rot_z(0)
{
    window = glfwCreateWindow(640, 480, "Terrain", nullptr, nullptr);

    if (window == nullptr)
    {
        *b_ret = 1;

        return;
    }

    glfwSetWindowSizeCallback(window, glfw_window_resize_callback);

    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_LIGHTING);

    refresh_view();

}

Renderer::~Renderer()
{

}

void Renderer::refresh_view()
{
    int width, height;
    glfwGetWindowSize(getWindow(), &width, &height);


    float rtio = (float)width/(float)height;

    glMatrixMode(GL_PROJECTION);
    glViewport(0, 0, width, height);

    glLoadIdentity();
    glFrustum(-rtio, rtio, -1, 1, 1, 50000);

    for (int i = 0; i < 2; ++i)
    {
        glClearColor(135/255.f, 206/255.f, 235/255.f, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwSwapBuffers(getWindow());
    }

}

void Renderer::pre_render_steps()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(135/255.f, 206/255.f, 235/255.f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::post_render_steps()
{

}


void Renderer::render(GameObject* gm)
{

    if (gm->getVBO("gl_Vertex") == nullptr)
        return;

    glPushMatrix();

        //Prendre en compte la configuration de la caméra.
        Vector3 dvec = gm->getPosition()-getCameraPosition();

        glRotated(-getCameraRotation().x, 1, 0, 0);
        glRotated(-getCameraRotation().y, 0, 1, 0);
        glRotated(-getCameraRotation().z, 0, 0, 1);
        glTranslated(dvec.x, dvec.y, -dvec.z);

        glRotated(gm->getRotation().x, 1, 0, 0);
        glRotated(gm->getRotation().y, 0, 1, 0);
        glRotated(gm->getRotation().z, 0, 0, 1);

        gm->getVBO("gl_Vertex")->bind_vbo();

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


        if (gm->getVBO("gl_Color") != nullptr)
        {
            gm->getVBO("gl_Color")->bind_vbo();

            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

        if (gm->wasProgramInitialized())
        {
            gm->getShaderProgram().useProgram();
            GLfloat projection[16], modelview[16];
            glGetFloatv(GL_PROJECTION_MATRIX, projection);
            glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

            glUniformMatrix4fv( glGetUniformLocation(gm->getShaderProgram().getID(), "projection"), 1, GL_FALSE, projection );
            glUniformMatrix4fv( glGetUniformLocation(gm->getShaderProgram().getID(), "modelview"), 1, GL_FALSE, modelview );
            glUniform1f( glGetUniformLocation(gm->getShaderProgram().getID(), "time"), (float)glfwGetTime() );

        }

        glDrawArrays(GL_TRIANGLES, 0, gm->getVBO("gl_Vertex")->getDataCount()/3);

            if (gm->wasProgramInitialized())
        gm->getShaderProgram().stopUseProgram();

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(3);


    glPopMatrix();


}

Vector3 Renderer::getCameraPosition(){
    return Vector3(cam_pos_x,cam_pos_y,cam_pos_z);
}

Vector3 Renderer::getCameraRotation(){
    return Vector3(cam_rot_x,cam_rot_y,cam_rot_z);
}
