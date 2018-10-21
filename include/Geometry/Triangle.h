#pragma once
#include "glm/glm.hpp"
#include <iostream>
#include <vector>


class Triangle {
public:
	//create a triangle from p1,p2,p3 and orient it so that the normal is facing away from interior point
	Triangle(glm::vec3 _p1, glm::vec3 _p2, glm::vec3 _p3, glm::vec3 _interior);

	//returning points on the triangle
	glm::vec3 getPoint(int i);//i is 0,1,2
	std::vector< glm::vec3 > getPoints();

	//returning a buffer of 18 floats in the form x1,y1,z1,normalx,normaly,normalz.... x3,y3,z3,normalx,normaly,normalz
	void addBuffer(std::vector< float >* buffer, bool withNormals = true);

	//other info
	glm::vec3 getNormal();
	glm::vec3 getInterior();

private:
	glm::vec3 p1, p2, p3, interior, normal;
};
