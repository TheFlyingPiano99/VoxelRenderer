#version 420 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

// Outputs the current position for the Fragment Shader
out vec3 worldPos;

// Imports the camera matrix from the main function
struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera camera;

uniform mat4 modelMatrix;


void main()
{
	worldPos = (modelMatrix * vec4(aPos, 1)).xyz;	
	gl_Position = camera.viewProjMatrix * modelMatrix * vec4(aPos, 1.0);
}