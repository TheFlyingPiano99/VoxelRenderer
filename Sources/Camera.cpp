#include"Camera.h"
#include <iostream>

Camera::Camera(int width, int height, glm::vec3 position, glm::vec3 lookDir)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
	this->lookDir = lookDir;

}

void Camera::updateMatrix()
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);


	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + lookDir, prefUp);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;

	//Inverse matrix:
	glm::mat4 invView = glm::mat4(1.0f);
	glm::mat4 invProjection = glm::mat4(1.0f);
	invView = glm::inverse(view);			// Should be imporved!
	invProjection = glm::inverse(projection);	// Should be imporved!
	invCameraMatrix = invView * invProjection;
}

void Camera::updateOrientation(glm::vec3 newPrefUp)
{
	glm::vec3 right = glm::cross(lookDir, newPrefUp);
	lookDir = glm::cross(newPrefUp, right);
	prefUp = newPrefUp;
}

void Camera::exportMatrix(Shader& shader)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camera.invViewProjMatrix"), 1, GL_FALSE, glm::value_ptr(invCameraMatrix));
}


void Camera::exportData(Shader& shader)
{
	glUniform3f(glGetUniformLocation(shader.ID, "camera.eye"), Position.x, Position.y, Position.z);
	glm::vec3 center = Position + lookDir;
	glUniform3f(glGetUniformLocation(shader.ID, "camera.center"), center.x, center.y, center.z);
	glm::vec3 right = normalize(cross(lookDir, prefUp));
	glUniform3f(glGetUniformLocation(shader.ID, "camera.right"), right.x, right.y, right.z);
	glm::vec3 up = normalize(cross(right, lookDir));
	glUniform3f(glGetUniformLocation(shader.ID, "camera.up"), up.x, up.y, up.z);
	glUniform1f(glGetUniformLocation(shader.ID, "camera.FOVrad"), glm::radians(FOVdeg));
	glUniform1f(glGetUniformLocation(shader.ID, "camera.aspectRatio"), width / (float)height);
	exportMatrix(shader);
}

void Camera::exportPostprocessDataAsLightCamera(Shader& shader)
{
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.eye"), Position.x, Position.y, Position.z);
	glm::vec3 center = Position + lookDir;
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.center"), center.x, center.y, center.z);
	glm::vec3 right = normalize(cross(lookDir, prefUp));
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.right"), right.x, right.y, right.z);
	glm::vec3 up = normalize(cross(right, lookDir));
	glUniform3f(glGetUniformLocation(shader.ID, "lightCamera.up"), up.x, up.y, up.z);
	glUniform1f(glGetUniformLocation(shader.ID, "lightCamera.FOVrad"), glm::radians(FOVdeg));
	glUniform1f(glGetUniformLocation(shader.ID, "lightCamera.aspectRatio"), width / (float)height);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightCamera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "lightCamera.invViewProjMatrix"), 1, GL_FALSE, glm::value_ptr(invCameraMatrix));
}


void Camera::Inputs(GLFWwindow* window)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * lookDir;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(lookDir, prefUp));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -lookDir;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(lookDir, prefUp));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * prefUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -prefUp;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}



	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{

		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(lookDir, glm::radians(-rotX), glm::normalize(glm::cross(lookDir, prefUp)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, prefUp) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			lookDir = newOrientation;
		}

		// Rotates the Orientation left and right
		lookDir = glm::rotate(lookDir, glm::radians(-rotY), prefUp);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}

// Not used yet:

void Camera::moveForward(float dt) {
	glm::vec3 right = glm::normalize(glm::cross(lookDir, prefUp));
	Position += dt * speed * glm::cross(prefUp, right);
}

void Camera::moveBackward(float dt)
{
	glm::vec3 right = glm::normalize(glm::cross(lookDir, prefUp));
	Position += dt * speed * -glm::cross(prefUp, right);
}

void Camera::moveLeft(float dt)
{
	Position += dt * speed * -glm::normalize(glm::cross(lookDir, prefUp));
}

void Camera::moveRight(float dt)
{
	Position += dt * speed * glm::normalize(glm::cross(lookDir, prefUp));
}

void Camera::moveUp(float dt)
{
	Position += dt * speed * prefUp;
}

void Camera::moveDown(float dt)
{
	Position += dt * speed * -prefUp;
}

void Camera::rotate(float mouseX, float mouseY)
{
	// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
	// and then "transforms" them into degrees 
	float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
	float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

	// Calculates upcoming vertical change in the Orientation
	glm::vec3 newOrientation = glm::rotate(lookDir, glm::radians(-rotX), glm::normalize(glm::cross(lookDir, prefUp)));

	// Decides whether or not the next vertical Orientation is legal or not
	if (abs(glm::angle(newOrientation, prefUp) - glm::radians(90.0f)) <= glm::radians(85.0f))
	{
		lookDir = newOrientation;
	}

	// Rotates the Orientation left and right
	lookDir = glm::rotate(lookDir, glm::radians(-rotY), prefUp);

}

void Camera::setPosition(glm::vec3 pos)
{
	Position = pos;
}
