#version 420 core

layout (location = 0) out vec4 FragColor;

in vec3 worldPos;

void main()
{
	FragColor = vec4(worldPos, 1.0f);
}