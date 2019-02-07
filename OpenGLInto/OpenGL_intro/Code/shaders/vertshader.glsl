#version 330 core
// Input attributes
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 color;

// Output the interpolatedColor for the fragshader
out vec3 interpolatedColor;

void main()
{
    interpolatedColor = color;
    gl_Position = vec4(position,0,1);
}
