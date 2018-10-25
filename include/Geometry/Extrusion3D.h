#pragma once
#include "include/Geometry/Object3DBase.h"
#include "include/Geometry/PolygonBuilder.h"

class Extrusion3D : public Object3DBase {

public:
	Extrusion3D();
	Extrusion3D(myPolygon poly1, myPolygon poly2, unsigned int nInterpolations = 1, int axis = 2, float x = 0, 
		float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1);
	~Extrusion3D();

	void setupBuffers(myPolygon poly1, myPolygon poly2, unsigned int nInterpolations = 0);

	//bool insideBounds(glm::vec3 v);
private:
	int specialFaceAxis;//x=0, y=1, z=2
};