#include "include/Geometry/Tube3D.h"

Tube3D::Tube3D(myPolygon poly1, myPolygon poly2, unsigned int nInterpolations, int axis, float x, float y,
	float z, float Lx, float Ly, float Lz)
	:specialFaceAxis(axis)
{
	if (poly1.getNSides() != poly2.getNSides()) {
		std::cout << "WARNING, Tube3D REQUIRES 2 POLYGONS HAVING SAME NUMBER OF POINTS" << std::endl;
	}

	isHollow = true;

	setupBuffers(poly1, poly1, poly2, poly2, nInterpolations);

	//uniforms
	setOffXYZ(x, y, z);
	setDimXYZ(Lx, Ly, Lz);
	if (specialFaceAxis == 0) setRotXYZ(0, 0, 0);
	else if (specialFaceAxis == 1) setRotXYZ(0, 0, 3.14159 / 2.0);
	else setRotXYZ(0, 3.14159 / 2.0, 0);
}

Tube3D::Tube3D(myPolygon poly1, myPolygon poly2, myPolygon poly3, myPolygon poly4, unsigned int nInterpolations, int axis, float x, float y,
	float z, float Lx, float Ly, float Lz)
	:specialFaceAxis(axis)
{
	if (poly1.getNSides() != poly2.getNSides()) {
		std::cout << "WARNING, Tube3D REQUIRES 2 POLYGONS HAVING SAME NUMBER OF POINTS" << std::endl;
	}

	isHollow = true;

	setupBuffers(poly1, poly2, poly3, poly4, nInterpolations);

	//uniforms
	setOffXYZ(x, y, z);
	setDimXYZ(Lx, Ly, Lz);
	if (specialFaceAxis == 0) setRotXYZ(0, 0, 0);
	else if (specialFaceAxis == 1) setRotXYZ(0, 0, 3.14159 / 2.0);
	else setRotXYZ(0, 3.14159 / 2.0, 0);
}

void Tube3D::setupBuffers(myPolygon poly1, myPolygon poly2, myPolygon poly3, myPolygon poly4, unsigned int nInterpolations) {
	glm::vec3 c = glm::vec3(0);//center of object in model coordinates
	PolygonBuilder polyBuilder = PolygonBuilder();
	
	//vertical face on x+ side
	std::vector< myPolygon > tempPoly1 = polyBuilder.tube2D(poly1, 0.5, poly3, 0.5, 0, false);
	polygons.insert(std::end(polygons), std::begin(tempPoly1), std::end(tempPoly1));

	//vertical face on x- side
	std::vector< myPolygon > tempPoly2 = polyBuilder.tube2D(poly2, -0.5, poly4, -0.5, 0, false);
	polygons.insert(std::end(polygons), std::begin(tempPoly2), std::end(tempPoly2));

	//outside face connecting 2 sides
	std::vector< myPolygon > tempPoly3 = polyBuilder.tube2D(poly3, 0.5, poly4, -0.5, nInterpolations, false);
	polygons.insert(std::end(polygons), std::begin(tempPoly3), std::end(tempPoly3));

	//inside face connecting 2 sides
	std::vector< myPolygon > tempPoly4 = polyBuilder.tube2D(poly1, 0.5, poly2, -0.5, nInterpolations, true);
	polygons.insert(std::end(polygons), std::begin(tempPoly4), std::end(tempPoly4));

	std::vector< float > positions;
	for (unsigned int i = 0; i < polygons.size(); i++) polygons[i].addBuffer(&positions, true);

	boxVertexBuffer = new VertexBuffer(positions.data(), positions.size() * sizeof(float));
	boxVertexArray.AddBuffer(*boxVertexBuffer, boxBufferLayout);

	setNVertices(positions.size() / 6);
}

Tube3D::~Tube3D() {
	delete boxVertexBuffer;
}
/*
//this will not work as it currently is
bool Tube3D::insideBounds(glm::vec3 v) {
	if (fabs(v.z) > 0.5) return false;
	for (size_t i = 0; i < polygons.size(); i++) {
		if (glm::dot(polygons[i].getNormal(), polygons[i].vtxs[0] - v) < 0) return false;
	}
	return true;
}

//figure out what to do here
bool Tube3D::isInsideHollow(glm::vec3 v, float R, std::vector< myPolygon >* polys, glm::mat4 preTransform) {
	return false;
}*/