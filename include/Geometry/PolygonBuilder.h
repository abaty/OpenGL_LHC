#pragma once
#include "include/Geometry/myPolygon.h"
#ifndef PI
	#define PI 3.14592f
#endif

class PolygonBuilder {
public:
	//baseRatio should be less than 1 (small/large)
	myPolygon Rectangle(float w, float h, float z, glm::vec3 center = glm::vec3(0));
	myPolygon Trapezoid(float baseRatio, float z, float avgWidth = 1, float avgHeight = 1, glm::vec3 center = glm::vec3(0));
	myPolygon nGon(unsigned int nPoints, float z,  float radius = 1, float dPhi = 0, glm::vec3 center = glm::vec3(0));
	myPolygon Sector(unsigned int nPoints, float z, float radius = 1, float dPhi = PI/2, glm::vec3 center = glm::vec3(0));
	std::vector< myPolygon > tube2D(myPolygon poly1, float z1 ,myPolygon poly2, float z2 ,bool flipFaceOrientation = false);
	std::vector< myPolygon > tube2D(myPolygon poly1, float z1, myPolygon poly2, float z2, unsigned int nInterpolations = 1,
		bool flipFaceOrientation = false);
};