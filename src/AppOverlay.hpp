#ifndef CM3D_APP_OVERLAY_HPP
#define CM3D_APP_OVERLAY_HPP

#include "cmcommon.h"
#include "FPCamera.hpp"

#include <GLFW/glfw3.h>

#include <vector>

NAMESPACE_BEGIN(cm3d)

class World;

class Overlay
{
public:
	// Init GLFW, create window with given parameters, then load GL.
	// If width or height == 0, then apply from videomode of current monitor.
	Overlay(World* world, int width, int height, const char *title,
		GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL);
	
	int initGLFW(GLFWmonitor* monitor = NULL);
	int loadGL();
	
	void linkWorld(World* world);
	void linkCamera(Camera *cam);
	void unlinkCamera(Camera *cam);
	void switchCamera(Camera *cam);

	void processInput();
	void renderFrame();
	
	GLFWwindow* context;
protected:
	World* linkedWorld;
	Camera* currentCamera;
	std::vector<Camera*> cameras;
private:
	bool viewChanged = false;
	int winWidth, winHeight;
}

NAMESPACE_END(cm3d)

#endif