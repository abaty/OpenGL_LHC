#include "include/Geometry/Triangle.h"


//creates a triangle w correct point ordering so that the normal is facing away from the point 'interior'
//also stores the direction of this normal vector
Triangle::Triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _interior) :
	p1(_p1), p2(_p2), p3(_p3), interior(_interior)
{
	//set normal as the cross product of the 2 sides of the triangle
	normal = glm::normalize(glm::cross(p2 - p1, p3 - p2));

	//if the normal and the vector from the interior to the first point are is not in the same hemisphere
	//then we reverse the normal direction and swap the order of p2 and p3 to give correct face orientation
	if (glm::dot(normal, p1 - interior) < 0) {
		normal = -normal;
		glm::vec3 tempV = p3;
		p3 = p2;
		p2 = tempV;
	}
}

glm::vec3 Triangle::getPoint(int i) {
	if (i == 0) return p1;
	if (i == 1) return p2;
	if (i == 2) return p3;

	std::cout << "argument in getPoint() is not 0,1,or 2!" << std::endl;
	return glm::vec3(0);
}

std::vector< glm::vec3 > Triangle::getPoints() {
	std::vector< glm::vec3 > p;
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	return p;
}

glm::vec3 Triangle::getNormal() {
	return normal;
}

glm::vec3 Triangle::getInterior() {
	return interior;
}

void Triangle::addBuffer(std::vector< float >* buffer, bool withNormals) {
	for (int i = 0; i < 3; i++) buffer->push_back(p1[i]);
	if (withNormals) {
		for (int i = 0; i < 3; i++) buffer->push_back(normal[i]);
	}
	for (int i = 0; i < 3; i++) buffer->push_back(p2[i]);
	if (withNormals) {
		for (int i = 0; i < 3; i++) buffer->push_back(normal[i]);
	}
	for (int i = 0; i < 3; i++) buffer->push_back(p3[i]);
	if (withNormals) {
		for (int i = 0; i < 3; i++) buffer->push_back(normal[i]);
	}
}
