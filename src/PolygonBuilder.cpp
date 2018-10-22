#include "include/Geometry/PolygonBuilder.h"

myPolygon PolygonBuilder::Trapezoid(float baseRatio, float z, glm::vec3 center) {
	float x = (1-baseRatio)/(1+baseRatio);
	
	std::vector< glm::vec3 > poly = {
		glm::vec3(-0.5f-x, -0.5f, z),
		glm::vec3(0.5f+x, -0.5f, z),
		glm::vec3(0.5f-x, 0.5f, z),
		glm::vec3(-0.5f+x, 0.5f, z)
	};
	return myPolygon(poly, center);
}

myPolygon PolygonBuilder::nGon(unsigned int nPoints, float z, glm::vec3 center) {
	if (nPoints < 3) {
		std::cout << "NGON must be more than 2 points!" << std::endl;
	}
	std::vector< glm::vec3 > poly;
	for (unsigned int i = 0; i < nPoints; i++) {
		float phi = i * 2 * 3.141592 / nPoints;
		poly.push_back(glm::vec3(0.5*cos(phi),0.5*sin(phi),z));
	}
	return myPolygon(poly, center);
}

myPolygon PolygonBuilder::nGonPhiOffset(unsigned int nPoints, float z, glm::vec3 center) {
	if (nPoints < 3) {
		std::cout << "NGON must be more than 2 points!" << std::endl;
	}

	std::vector< glm::vec3 > poly;
	for (unsigned int i = 0; i < nPoints; i++) {
		float phi = (i+0.5) * 2 * 3.141592 / nPoints;
		poly.push_back(glm::vec3(0.5*cos(phi), 0.5*sin(phi), z));
	}
	return myPolygon(poly, center);
}