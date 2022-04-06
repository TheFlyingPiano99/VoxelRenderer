#version 420 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

struct Camera {
    mat4 viewProjMatrix;
};
uniform Camera camera;

void main()
{
    TexCoords = aPos;
    gl_Position = camera.viewProjMatrix * vec4(aPos, 1.0);
} 