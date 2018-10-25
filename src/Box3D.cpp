#include "include/Geometry/Box3D.h"

Box3D::Box3D(float x, float y, float z, float Lx, float Ly, float Lz) {
	glm::vec3 c = glm::vec3(0);//center of object in model coordinates

	std::vector< glm::vec3 > side[6];
	side[0] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f)
	};
	polygons.push_back(myPolygon(side[0], c));

	side[1] = {
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f)
	};
	polygons.push_back(myPolygon(side[1], c));

	setupBuffers(polygons[0], polygons[1]);

	//uniforms
	setOffXYZ(x, y, z);
	setDimXYZ(Lx, Ly, Lz);
	setRotXYZ(0, 0, 0);
}

Box3D::~Box3D() {
	delete boxVertexBuffer;
}
/*
bool Box3D::insideBounds(glm::vec3 v) {
	return fabs(v.x) < 0.5 && fabs(v.y) < 0.5 && fabs(v.z) < 0.5;
}

void Box3D::updateSphereRadius() {
	setSphereRadius((float)pow(getDimX() * getDimX() + getDimY() * getDimY() + getDimZ() * getDimZ(), 0.5) / 2.0);
}*/
