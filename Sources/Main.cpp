

#include "Callbacks.h"
#include "Scene.h"
#include "GlobalInclude.h"

#include "GUI.h"

#define PROJECT_NAME "AtmosphereSim"

int initWindow(GLFWwindow*& window) {
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	window = glfwCreateWindow(windowWidth, windowHeight, "AtmosphereSim", NULL, NULL);
	// Error check if the window fails to create

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	Callbacks::setCallbacks(window);
	return 0;
}


int main()
{
	GLFWwindow* window;
	if (-1 == initWindow(window)) {
		return -1;
	}

	GUI::getInstance()->initGUI(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();

	Callbacks::onWindowInit(window);

	// Variables to create periodic event for FPS displaying
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	double prevIterTime = 0.0f;
	// Keeps track of the amount of frames in timeDiff
	unsigned int counter = 0;

	const double dtLimit = 10.0;

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (counter >= 500)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = "AtmosphereSim   (" + FPS + " FPS / " + ms + "ms)";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			prevTime = crntTime;
			counter = 0;
		}

		double dt = 0.0;
		double realDelta = (crntTime - prevIterTime) * 1000;
		//Scene::getInstance()->getCamera()->Inputs(window);		<<-- temp solution only!!!
		while (realDelta > 0.0) {
			if (realDelta > dtLimit) {
				dt = dtLimit;
				realDelta -= dtLimit;
			}
			else {
				dt = realDelta;
				realDelta = 0.0;
			}
			Scene::getInstance()->control(dt);
			Scene::getInstance()->animate(dt);
		}
		prevIterTime = crntTime;

		Callbacks::onWindowRefresh(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}

	GUI::getInstance()->destroyGUI();

	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}