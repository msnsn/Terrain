#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "root.hpp"
#include "renderer.hpp"


#include <iostream>

using namespace std;



int main()
{
    int b_ret = 0;
    delete new Root(&b_ret);
    return b_ret;
}
