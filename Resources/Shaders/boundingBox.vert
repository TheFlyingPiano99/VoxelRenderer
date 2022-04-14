#version 420 core

layout (location = 0) in vec3 aPos;

out vec3 modelPos;
out vec2 quadTexCoords;

struct Camera {
	mat4 viewProjMatrix;
};
uniform Camera camera;

struct SceneObject {
	mat4 modelMatrix;
	mat4 invModelMatrix;
};
uniform SceneObject sceneObject;


void main()
{
	modelPos = aPos;
	vec4 homogeneCamPos = camera.viewProjMatrix * sceneObject.modelMatrix * vec4(aPos, 1.0);
	gl_Position = homogeneCamPos;
	quadTexCoords = ((homogeneCamPos / homogeneCamPos.w).xy + 1) * 0.5;
}