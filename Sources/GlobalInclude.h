#pragma once
#include <GLFW/glfw3.h>

#define DEFAULT_WINDOW_WIDTH 1024
#define DEFAULT_WINDOW_HEIGHT 768

#define FBO_WIDTH 1920
#define FBO_HEIGHT 1080

#define SHADOW_WIDTH 2000
#define SHADOW_HEIGHT 2000

static struct GlobalVariables {
	static GLFWwindow* window;
	static int windowWidth;
	static int windowHeight;
	static bool fullScreenMode;
};