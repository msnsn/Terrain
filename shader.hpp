#pragma once

#include <string>

typedef unsigned int GLenum;
typedef unsigned int GLuint;

class Shader
{
    public:
        Shader(std::string src, GLenum type);
        virtual ~Shader();

        inline const GLuint getID() const{return id;}
        inline const GLenum getType() const{return type;}

        void clearShader();

    protected:
        GLuint id;
        GLenum type;

        std::string src = "";
};
