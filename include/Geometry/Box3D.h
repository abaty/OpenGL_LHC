#pragma once
#include "include/Geometry/Prism3D.h"

class Box3D: public Prism3D {

public:
	Box3D(float x = 0, float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1);
	~Box3D();

private:
	//function for updating sphere radius quickly
	void updateSphereRadius();

	//helper function for determining if a point is inside the model (in model coordinates)
	bool insideBounds(glm::vec3);
};