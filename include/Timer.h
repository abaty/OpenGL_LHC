#pragma once
#include <GLFW/glfw3.h>

class Timer {
public:

	Timer() { start = glfwGetTime(); }

	double TimeSplit() { return glfwGetTime() - start; }
	double Restart() {
		double timeDifference = TimeSplit();
		start = glfwGetTime();
		return timeDifference;
	}

private:
	double start;
};