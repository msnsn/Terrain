#pragma once

typedef unsigned int GLenum;
typedef unsigned int GLuint;

class Shader;

class Program
{
public:
    Program();
    virtual ~Program();

    void attachShader(const Shader& shader);
    void link();
    void useProgram();
    void stopUseProgram();
    void clearProgram();

    inline const GLuint getID() const{return id;}

protected:
    GLuint id;

};
