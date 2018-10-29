#pragma once
#include "include/Geometry/Object3DBase.h"
#include "include/Geometry/PolygonBuilder.h"
#include "include/Geometry/Extrusion3D.h"

class Tube3D : public Object3DBase {

public:
	//x+ inner, x- inner, x+ outer, x- outer
	Tube3D(myPolygon poly1, myPolygon poly2, myPolygon poly3, myPolygon poly4, unsigned int nInterpolations = 1, int axis = 2, float x = 0,
		float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1);
	//x+ inner, x+ outer
	Tube3D(myPolygon poly1, myPolygon poly2, unsigned int nInterpolations = 1, int axis = 2, float x = 0,
		float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1);
	~Tube3D();

	void setupBuffers(myPolygon poly1, myPolygon poly2, myPolygon poly3, myPolygon poly4, unsigned int nInterpolations = 0);

	bool isInsideHollowRegion(glm::vec3 v);

private:
	//this is not displayed but is used for collision detection
	Extrusion3D innerVolume;

	void updateInnerCylinderRadius(myPolygon poly1, myPolygon poly2);
	float innerCylinderRadiusPlus;
	float innerCylinderRadiusMinus;

	int specialFaceAxis;//x=0, y=1, z=2
};