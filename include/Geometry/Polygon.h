#pragma once
#include "include/Geometry/Triangle.h"
#include <vector>

//class for a planar shape
class Polygon {
public:
	Polygon(std::vector< glm::vec3 > vtxs, glm::vec3 interior);
	void addBuffer(std::vector< float > * buffer, bool withNormals = true);

	std::vector< Triangle > polygonTriangles;

	float getCollisionSphereRadius() { return sphereRadius; };
	float getScaledCollisionSphereRadius(glm::vec3 point);
	glm::vec3 getCollisionSphereCenter() { return sphereCenter; };

private:
	//stuff for collision detection
	//define a sphere that contains all the points of the object
	glm::vec3 sphereCenter;
	float sphereRadius;
	glm::vec3 normal;
};
