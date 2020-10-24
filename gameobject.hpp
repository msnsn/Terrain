#pragma once

#include <vector>
#include <string>
#include "program.hpp"

typedef unsigned int GLenum;
class Root;
class VBO;
class Vector3;

struct StoredVBO
{
    std::string identifier = "";
    VBO* vbo = nullptr;
};


class GameObject
{
public:
    GameObject(Root* c_root);
    virtual ~GameObject();

    VBO* getVBO(std::string id);

    void createVBO(std::string id, GLenum type);
    void deleteVBO(std::string id);
    void clearVBOs();


    Vector3 getPosition();
    Vector3 getRotation();

    virtual void update(float deltaTime){
    }

    inline void setPosition(float x, float y, float z)
    {
        pos_x = x;
        pos_y = y;
        pos_z = z;
    }

    inline void setRotation(float x, float y, float z)
    {
        rot_x = x;
        rot_y = y;
        rot_z = z;
    }

    inline void setShaderProgram(const Program& program)
    {
        _programInitialized = true;
        shaderProgram = program;
    }

    inline Program getShaderProgram() const{return shaderProgram;}

    inline const bool wasProgramInitialized() const {return _programInitialized;}

protected:
    Root* root;
    VBO* vbo;
    Program shaderProgram;
    bool _programInitialized;

    float pos_x, pos_y, pos_z;
    float rot_x, rot_y, rot_z;

    std::vector<StoredVBO*> storedVBOs;


};
