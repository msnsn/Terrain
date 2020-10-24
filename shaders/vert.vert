#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 4) in vec3 aColor;

uniform mat4 projection;
uniform mat4 modelview;
uniform float time;

out vec3 color;

void main()
{
    gl_Position = projection * modelview * vec4(aPos, 1.0);
    color = aColor;
}

