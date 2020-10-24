#include "gameobject.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "root.hpp"
#include "renderer.hpp"
#include "vbo.hpp"
#include "vector3.hpp"
#include "program.hpp"

GameObject::GameObject(Root* c_root) :
    root(c_root),
    storedVBOs(std::vector<StoredVBO*>()),
    pos_x(0),
    pos_y(0),
    pos_z(0),
    rot_x(0),
    rot_y(0),
    rot_z(0),
    _programInitialized(false)
{
    root->registerGameObject(this);

}

GameObject::~GameObject()
{
    root->unregisterGameObject(this);

    clearVBOs();
}


Vector3 GameObject::getPosition(){
    return Vector3(pos_x,pos_y,pos_z);
}

Vector3 GameObject::getRotation(){
    return Vector3(rot_x,rot_y,rot_z);
}

VBO* GameObject::getVBO(std::string id)
{
    std::vector<StoredVBO*>::iterator it;
    for (it = storedVBOs.begin(); it != storedVBOs.end(); ++it)
    {
        if ((*it)->identifier == id)
            return (*it)->vbo;
    }
    return nullptr;
}

void GameObject::createVBO(std::string id, GLenum type)
{
    if (getVBO(id) != nullptr)
        deleteVBO(id);
    VBO* vbo = new VBO(type);
    StoredVBO* sVBO = new StoredVBO();
    sVBO->identifier = id;
    sVBO->vbo = vbo;
    storedVBOs.push_back(sVBO);
}

void GameObject::deleteVBO(std::string id){
    VBO* vbo = getVBO(id);
    if (vbo != nullptr)
    {
        std::vector<StoredVBO*>::iterator it;
        for (it = storedVBOs.begin(); it != storedVBOs.end(); ++it)
        {
            if ((*it)->identifier == id)
                {
                    storedVBOs.erase(it);
                    delete vbo;
                    return;
                }
        }
    }
}

void GameObject::clearVBOs()
{
    std::vector<StoredVBO*>::iterator it;
    for (it = storedVBOs.begin(); it != storedVBOs.end(); ++it)
        if ((*it)->vbo != nullptr)
            delete (*it)->vbo;

    storedVBOs.clear();
}
