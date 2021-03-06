#include "Callbacks.h"
#include "Scene.h"
#include "ControlActionManager.h"
#include "SingletonManager.h"
#include "GlobalInclude.h"
#include "GUI.h"

// Prevents the camera from jumping around when first clicking left click
bool firstClick = true;


void Callbacks::setCallbacks(GLFWwindow* window) {
	glfwSetWindowRefreshCallback(window, Callbacks::onWindowRefresh);
	glfwSetWindowCloseCallback(window, Callbacks::onWindowClose);
	glfwSetKeyCallback(window, Callbacks::onKey);
	glfwSetCursorPosCallback(window, Callbacks::onMouseMove);
	glfwSetScrollCallback(window, Callbacks::onMouseScroll);
	glfwSetMouseButtonCallback(window, Callbacks::onMouseClick);

}

void Callbacks::onWindowInit(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// Set control layout
	ControlActionManager::getInstance()->registerDefault();

	Scene::getInstance()->init(width, height);
	GUI::getInstance()->initGUI(window);
}


void Callbacks::onWindowRefresh(GLFWwindow* window)
{
	GUI::getInstance()->preDrawInit();

	Scene::getInstance()->draw();

	GUI::getInstance()->configToScene(*Scene::getInstance());
	GUI::getInstance()->draw();

	glfwSwapBuffers(window);
}


void Callbacks::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ControlActionManager::getInstance()->onKey(key, scancode, action, mods);
}


void Callbacks::onMouseMove(GLFWwindow* window, double xpos, double ypos)
{

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{

		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (GlobalVariables::windowWidth / 2), (GlobalVariables::windowHeight / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);


		Scene::getInstance()->getCamera()->rotateAroundBullseye(mouseX, mouseY, Scene::getInstance()->getVoxelData()->getPosition());


		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (GlobalVariables::windowWidth / 2), (GlobalVariables::windowHeight/ 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}

}

void Callbacks::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	Scene::getInstance()->getCamera()->approachCenter(yoffset);
}

void Callbacks::onMouseClick(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = xpos / GlobalVariables::windowWidth * 2.0 - 1.0;
		ypos = 1.0 - ypos / GlobalVariables::windowHeight * 2.0;
		Scene::getInstance()->getVoxelData()->selectTransferFunctionRegion(xpos, ypos);
	}
}

void Callbacks::toggleFullScreen()
{
	GlobalVariables::fullScreenMode = !GlobalVariables::fullScreenMode;
	if (GlobalVariables::fullScreenMode) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(GlobalVariables::window, monitor, 0, 0, mode->width, mode->height, 60);
	}
	else {
		glfwSetWindowMonitor(GlobalVariables::window, nullptr, 50, 50, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 60);
	}
	glfwGetWindowSize(GlobalVariables::window, &GlobalVariables::windowWidth, &GlobalVariables::windowHeight);
	glViewport(0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight);
	std::cout << "Screen size: " << GlobalVariables::windowWidth << ", " << GlobalVariables::windowHeight << std::endl;
	Scene::getInstance()->onContextResize(GlobalVariables::windowWidth, GlobalVariables::windowHeight);
}



void Callbacks::onWindowClose(GLFWwindow* window)
{
	// Delete all the objects we've created

	Singleton::destroyAllInstances();
}
