#version 330 core


in vec3 color;
in vec4 gl_FragCoord;

void main()
{
    gl_FragColor = new vec4(color.r, color.g, color.b, color.a);
}
