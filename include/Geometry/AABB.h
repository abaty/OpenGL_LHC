#pragma once
#include "glm/glm.hpp"

class AABB {
public:
	AABB();
	AABB(glm::vec3 v);

	void update(glm::vec3 v);
	void reset(glm::vec3 v);

	bool isInside(float x, float y, float z);
	bool isInside(glm::vec3 v);
	bool isInside(AABB aabb);

	float getXmin() { return xmin; };
	float getXmax() { return xmax; };
	float getYmin() { return ymin; };
	float getYmax() { return ymax; };
	float getZmin() { return zmin; };
	float getZmax() { return zmax; };

private:
	float xmin, xmax, ymin, ymax, zmin, zmax;
};