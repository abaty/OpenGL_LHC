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

	innerVolume = Extrusion3D(poly1, poly1, nInterpolations, 0, x , y, z, Lx, Ly, Lz);
}

//inner x+, inner x-, outer x+, outer x-
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

	innerVolume = Extrusion3D(poly1, poly2, nInterpolations, 0, x, y, z, Lx, Ly, Lz);
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

	updateInnerCylinderRadius(poly1, poly2);
}

//returns true if the point is not in the model
bool Tube3D::isInsideHollowRegion(glm::vec3 v) {
	glm::vec3 modelPt = getInverseMatrix() * glm::vec4(v, 1.0f);
	if (fabs(modelPt.z) > 0.5) return true;

	//there is a cylinder w/ radius R(z) that is the maximum radius to still not contact the tube
	//see if we are inside this cylinder quickly
	float distFromZaxis = glm::length(glm::vec2(modelPt));
	float dz = 0.5 - modelPt.z;
	float maxAllowedDist = dz*innerCylinderRadiusMinus + (1-dz)*innerCylinderRadiusPlus;
	if ( distFromZaxis < maxAllowedDist) return true;

	if (innerVolume.isInside(modelPt)) return true;
	return false;
}

Tube3D::~Tube3D() {
	delete boxVertexBuffer;
}

void Tube3D::updateInnerCylinderRadius(myPolygon poly1, myPolygon poly2) {
	innerCylinderRadiusPlus = -1;

	//https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Another_vector_formulation
	//calculates distance to 0,0
	for (size_t k = 0; k < poly1.vtxs.size(); k++) {
		size_t nextIndex = (k == poly1.vtxs.size() - 1) ? 0 : k + 1;//wrap around if needed

		glm::vec3 point = poly1.vtxs[k];
		point.z = 0.0f;
		glm::vec3 pointNxt = poly1.vtxs[nextIndex];
		pointNxt.z = 0.0f;
		glm::vec3 u = glm::normalize(pointNxt - point);
		float d = glm::length(glm::cross(u, point));

		if (d > innerCylinderRadiusPlus) innerCylinderRadiusPlus = d;
	}

	//same code for minus
	innerCylinderRadiusMinus = -1;
	for (size_t k = 0; k < poly2.vtxs.size(); k++) {
		size_t nextIndex = (k == poly2.vtxs.size() - 1) ? 0 : k + 1;//wrap around if needed

		glm::vec3 point = poly2.vtxs[k];
		point.z = 0.0f;
		glm::vec3 pointNxt = poly2.vtxs[nextIndex];
		pointNxt.z = 0.0f;
		glm::vec3 u = glm::normalize(pointNxt - point);
		float d = glm::length(glm::cross(u, point));

		if (d < innerCylinderRadiusMinus || innerCylinderRadiusMinus<0) innerCylinderRadiusMinus = d;
	}
}