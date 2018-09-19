#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GlobalSettings {

public:
	//Graphics
	bool doAntiAliasing = true;

	//Drawing settings
	bool doDrawBeamline = true;

	//FPS counter
	bool doFPS = true;
	void getFPS();

private:
	//for FPS counter
	int nFrames = 0;
	double lastTime = 0;
};

extern GlobalSettings settings;