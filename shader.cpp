#include "shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <string>

#include <fstream>

#include <boost/filesystem.hpp>

Shader::Shader(std::string src_, GLenum type_) :
    src(src_),
    type(type_)
{
    id = glCreateShader(type);


    boost::filesystem::ifstream t(src);
    std::string datastr( (std::istreambuf_iterator<char>(t)),
                        std::istreambuf_iterator<char>() );

    t.close();

    const char* c_str;
    glShaderSource(id, 1, &(c_str=datastr.c_str()), NULL);
    glCompileShader(id);

    GLint isCompiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errlg(maxLength);
        glGetShaderInfoLog(id, maxLength, &maxLength, &errlg[0]);
        std::string err(errlg.begin(), errlg.end());
        std::cout << err << std::endl;
    }


}

Shader::~Shader()
{
}

void Shader::clearShader()
{
    glDeleteShader(id);
}
