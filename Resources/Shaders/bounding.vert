#version 420 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;

// Outputs the current position for the Fragment Shader
out vec3 modelPos;

// Imports the camera matrix from the main function
struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera camera;

struct SceneObject {
	mat4 modelMatrix;
};
uniform SceneObject sceneObject;


void main()
{
	modelPos = aPos;
	gl_Position = camera.viewProjMatrix * sceneObject.modelMatrix * vec4(aPos, 1.0);
}