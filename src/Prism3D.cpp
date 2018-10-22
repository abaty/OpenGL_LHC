#include "include/Geometry/Prism3D.h"

Prism3D::Prism3D() {}

Prism3D::Prism3D(myPolygon poly1, myPolygon poly2, int axis, float x, float y, float z, float Lx, float Ly, float Lz)
	:specialFaceAxis(axis)
	{
	if (poly1.getNSides() != poly2.getNSides()) {
		std::cout << "WARNING, PRISM3D REQUIRES 2 POLYGONS HAVING SAME NUMBER OF POINTS" << std::endl;
	}

	setupBuffers(poly1, poly2);

	//uniforms
	setOffXYZ(x, y, z);
	setDimXYZ(Lx, Ly, Lz);
	if(specialFaceAxis==0) setRotXYZ(0, 0, 0);
	else if (specialFaceAxis == 1) setRotXYZ(0, 0, 3.14159 / 2.0);
	else setRotXYZ(0, 3.14159/2.0, 0);
}

void Prism3D::setupBuffers(myPolygon poly1, myPolygon poly2) {

	glm::vec3 c = glm::vec3(0);//center of object in model coordinates

	//first polygon face
	std::vector< glm::vec3 > points1, points2;
	std::cout << poly1.vtxs.size() << std::endl;
	for (size_t j = 0; j < poly1.vtxs.size(); j++) {
		glm::vec3 poly1Vtx = poly1.vtxs[j];
		poly1Vtx.z = 0.5;
		points1.push_back(poly1Vtx);
	}

	//second polygon face
	for (size_t j = 0; j < poly2.vtxs.size(); j++) {
		glm::vec3 poly2Vtx = poly2.vtxs[j];
		poly2Vtx.z = -0.5;
		points2.push_back(poly2Vtx);
	}

	polygons.push_back(myPolygon(points1, c));
	polygons.push_back(myPolygon(points2, c));

	//take sides of poly1 and connect them to points on poly2
	for (size_t j = 0; j < poly1.vtxs.size(); j++) {
		std::vector< glm::vec3 > p;
		glm::vec3 v1 = poly1.vtxs[j];
		v1.z = 0.5;
		p.push_back(v1);
		glm::vec3 v2 = poly1.vtxs[(j + 1 == poly1.vtxs.size()) ? 0 : j + 1];//handles the wrap around case
		v2.z = 0.5;
		p.push_back(v2);
		glm::vec3 v3 = poly2.vtxs[j];
		v3.z = -0.5;
		p.push_back(v3);
		polygons.push_back(myPolygon(p, c));
	}
	//same for poly2
	for (size_t j = 0; j < poly2.vtxs.size(); j++) {
		std::vector< glm::vec3 > p;
		glm::vec3 v1 = poly2.vtxs[j];
		v1.z = -0.5;
		p.push_back(v1);
		glm::vec3 v2 = poly2.vtxs[(j + 1 == poly2.vtxs.size()) ? 0 : j + 1];//handles the wrap around case
		v2.z = -0.5;
		p.push_back(v2);
		glm::vec3 v3 = poly1.vtxs[(j + 1 == poly1.vtxs.size()) ? 0 : j + 1];
		v3.z = 0.5;
		p.push_back(v3);
		polygons.push_back(myPolygon(p, c));
	}

	std::vector< float > positions;
	for (unsigned int i = 0; i < polygons.size(); i++) polygons[i].addBuffer(&positions, true);

	boxVertexBuffer = new VertexBuffer(positions.data(), positions.size() * sizeof(float));
	boxVertexArray.AddBuffer(*boxVertexBuffer, boxBufferLayout);

	setNVertices(positions.size() / 6);
}

Prism3D::~Prism3D() {
	delete boxVertexBuffer;
}

bool Prism3D::insideBounds(glm::vec3 v) {
	if (fabs(v.z) > 0.5) return false;
	for (size_t i = 0; i < polygons.size(); i++) {
		if (glm::dot(polygons[i].getNormal(), polygons[i].vtxs[0] - v) < 0) return false;
	}
	return true;
}