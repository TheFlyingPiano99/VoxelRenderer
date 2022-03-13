#pragma once

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_HfirstC
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 lookDir = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 prefUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	glm::mat4 invCameraMatrix = glm::mat4(1.0f);
	float FOVdeg = 45.0f;
	float nearPlane = 2.0f;
	float farPlane = 2000.0f;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.8f;
	float sensitivity = 100.0f;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position, glm::vec3 lookDir);

	// Updates the camera matrix to the Vertex Shader
	void updateMatrix();

	void updateOrientation(glm::vec3 prefUp);
	// Exports the camera matrix to a shader
	void exportMatrix(Shader& shader);

	void exportData(Shader& program);
	void exportPostprocessDataAsLightCamera(Shader& program);

	// Obsolete
	// Handles camera inputs
	void Inputs(GLFWwindow* window);

	void moveForward(float dt);
	void moveBackward(float dt);
	void moveLeft(float dt);
	void moveRight(float dt);
	void moveUp(float dt);
	void moveDown(float dt);

	void rotate(float mouseX, float mouseY);

	void setPosition(glm::vec3 pos);
};
#endif