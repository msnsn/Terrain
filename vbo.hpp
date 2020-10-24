#pragma once

#include <vector>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

class VBO
{
public:
    VBO(GLenum type);
    virtual ~VBO();

    void buffer_data(float* data, int dataCount, GLenum usage);

    void bind_vbo();
    void unbind_vbo();

    inline int getDataCount()
    {
        return dataCount;
    }


protected:
    GLuint id;
    GLenum type;
    int dataCount = 0;
};
