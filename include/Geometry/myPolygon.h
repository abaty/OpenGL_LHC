#pragma once
#include "include/Geometry/Triangle.h"
#include <vector>

//class for a planar shape
class myPolygon {
public:
	myPolygon(std::vector< glm::vec3 > _vtxs, glm::vec3 interior, bool flipNormal = false);
	void addBuffer(std::vector< float > * buffer, bool withNormals = true);

	std::vector< Triangle > polygonTriangles;
	std::vector< glm::vec3 > vtxs;

	float getCollisionSphereRadius() { return sphereRadius; };
	float getScaledCollisionSphereRadius(glm::vec3 point);
	glm::vec3 getCollisionSphereCenter() { return sphereCenter; };

	glm::vec3 getNormal() { return normal; };

	//number of vertices in this polygon (not same as number of sides because it accounts for repeated vertices
	int getNVertices() { return nVertices; };
	//number of sides in this polygon (disregards repeated vertices)
	int getNSides() { return nVertices; };

	bool getIsNormalFlipped() { return isNormalFlipped; };

private:
	//stuff for collision detection
	//define a sphere that contains all the points of the object
	glm::vec3 sphereCenter;
	float sphereRadius;
	glm::vec3 normal;

	unsigned int nVertices;
	unsigned int nSides;

	bool isNormalFlipped = false;
};
