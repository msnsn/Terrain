#include "program.hpp"

#include <GL/glew.h>

#include "shader.hpp"

Program::Program()
{
    id = glCreateProgram();
}

Program::~Program()
{
}

void Program::clearProgram()
{
    glDeleteProgram(id);
}

void Program::attachShader(const Shader& shader)
{
    glAttachShader(getID(), shader.getID());
}

void Program::link()
{
    glLinkProgram(id);
}

void Program::useProgram()
{
    glUseProgram(id);
}

void Program::stopUseProgram()
{
    glUseProgram(0);
}
