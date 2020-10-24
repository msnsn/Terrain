#include "vbo.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

VBO::VBO(GLenum _type) : type(_type)
{
    glGenBuffers(1, &id);
}

VBO::~VBO()
{
    glDeleteBuffers(1, &id);
}

void VBO::buffer_data(float* data, int _dataCount, GLenum usage)
{
    dataCount = _dataCount;//sizeof(data)/sizeof(float);

    bind_vbo();
    glBufferData(type, dataCount * sizeof(float), data, usage);
}

void VBO::bind_vbo()
{
    glBindBuffer(type, id);
}

void VBO::unbind_vbo()
{
    glBindBuffer(type, 0);
}
