#include "include/Geometry/Polygon.h"

//assumes the points are given in a 'fan' configuration where they go Clockwise or CClockwise around the polygon
Polygon::Polygon(std::vector< glm::vec3 > vtxs, glm::vec3 interior) {
	if (vtxs.size() < 3) {
		std::cout << "Polygon::Polygon needs at least 3 points!" << std::endl;
		return;
	}

	sphereRadius = glm::distance(vtxs.at(0), vtxs.at(1))/2.0;
	sphereCenter = 0.5f * (vtxs.at(0) + vtxs.at(1));

	for (size_t i = 2; i < vtxs.size(); i++) {
		Triangle t = Triangle(vtxs.at(0), vtxs.at(i-1), vtxs.at(i), interior);
		
		//take normal from first triangle (all triangles should be coplanar)
		if (i == 2) normal = t.getNormal();

		polygonTriangles.push_back(t);

		for (size_t j = 0; j < i; j++) {
			float d = glm::distance(vtxs.at(j), vtxs.at(i)) / 2.0;
			if (d > sphereRadius) {
				sphereRadius = d;
				sphereCenter = 0.5f * (vtxs.at(i) + vtxs.at(j));
			}
		}
	}
}

void Polygon::addBuffer(std::vector< float > * buffer, bool withNormals) {
	for (size_t i = 0; i < polygonTriangles.size(); i++) {
		(polygonTriangles[i]).addBuffer(buffer, withNormals);
	}
}

float Polygon::getScaledCollisionSphereRadius(glm::vec3 point) {
	return 1-fabs(glm::dot(normal, glm::normalize(sphereCenter - point)));
}

