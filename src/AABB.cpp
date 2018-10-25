#include "include/Geometry/AABB.h"

AABB::AABB() {
	reset(glm::vec3(0));
}

AABB::AABB(glm::vec3 v) {
	reset(v);
}

void AABB::reset(glm::vec3 v) {
	xmin = v.x;
	xmax = v.x;
	ymin = v.y;
	ymax = v.y;
	zmin = v.z;
	zmax = v.z;
}

void AABB::update(glm::vec3 v) {
	if (v.x < xmin)  xmin = v.x;
	if (v.x > xmax)  xmax = v.x;
	if (v.y < ymin)  ymin = v.y;
	if (v.y > ymax)  ymax = v.y;
	if (v.z < zmin)  zmin = v.z;
	if (v.z > zmax)  zmax = v.z;
}

bool AABB::isInside(float x, float y, float z) {
	return isInside(glm::vec3(x, y, z));
}

bool AABB::isInside(glm::vec3 v) {
	if (v.x <= xmin) return false;
	if (v.x >= xmax) return false;
	if (v.y <= ymin) return false;
	if (v.y >= ymax) return false;
	if (v.z <= zmin) return false;
	if (v.z >= zmax) return false;
	return true;
}

bool AABB::isInside(AABB aabb) {
	if (aabb.getXmax() <= xmin) return false;
	if (aabb.getXmin() >= xmax) return false;
	if (aabb.getYmax() <= ymin) return false;
	if (aabb.getYmin() >= ymax) return false;
	if (aabb.getZmax() <= zmin) return false;
	if (aabb.getZmin() >= zmax) return false;
	return true;
}