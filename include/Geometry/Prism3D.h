#pragma once
#include "include/Geometry/Object3DBase.h"

class Prism3D : public Object3DBase {

public:
	//first polygon is placed on the x+ side
	//second polygon is placed on the x- side
	//first polygon is reused as 2nd if 2nd is not specified
	Prism3D();
	Prism3D(myPolygon poly1, myPolygon poly2, int axis = 2,float x = 0, float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1);
	Prism3D(myPolygon poly1, int axis = 2, float x = 0, float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1) : Prism3D(poly1, poly1, axis, x, y, z, Lx, Ly, Lz) {};//calls above constructor
	~Prism3D();

	void setupBuffers(myPolygon poly1, myPolygon poly2);

	bool insideBounds(glm::vec3 v);

private:
	int specialFaceAxis;//x=0, y=1, z=2
};