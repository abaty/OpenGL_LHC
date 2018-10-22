#pragma once
#include "include/Geometry/myPolygon.h"

class PolygonBuilder {
public:
	//baseRatio should be less than 1 (small/large)
	myPolygon Trapezoid(float baseRatio, float z, glm::vec3 center = glm::vec3(0));
	myPolygon nGon(unsigned int nPoints, float z, glm::vec3 center = glm::vec3(0));
	myPolygon nGonPhiOffset(unsigned int nPoints, float z, glm::vec3 center = glm::vec3(0));
};