#include "include/GlobalSettings.h"


void GlobalSettings::getFPS() {
	double currentTime = glfwGetTime();
	nFrames++;
	if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago printf and reset timer
		printf("%f ms/frame\n", 1000.0 / double(nFrames));
		nFrames = 0;
		lastTime = currentTime;
	}
}