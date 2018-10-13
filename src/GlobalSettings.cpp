#include "include/GlobalSettings.h"


float GlobalSettings::getFPS(bool doMSPerFrame) {
	double currentTime = glfwGetTime();
	nFrames++;
	if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago printf and reset timer
		FPS = float(nFrames);
		//printf("%f ms/frame\n", 1/FPS*1000.0);
		nFrames = 0;
		lastTime = currentTime;
	}

	if (doMSPerFrame) return 1 / FPS * 1000;
	return FPS;
}