#include "include/Geometry/PolygonBuilder.h"

myPolygon PolygonBuilder::Rectangle(float w, float h, float z, glm::vec3 center) {
	std::vector< glm::vec3 > poly = {
		glm::vec3(-w/2, -h/2, z),
		glm::vec3(w/2, -h/2, z),
		glm::vec3(w/2, h/2, z),
		glm::vec3(-w/2, h/2, z)
	};
	return myPolygon(poly, center);
}

myPolygon PolygonBuilder::Trapezoid(float baseRatio, float z, float avgWidth, float avgHeight, glm::vec3 center) {
	float x = (1-baseRatio)/(1+baseRatio);
	
	std::vector< glm::vec3 > poly = {
		glm::vec3(avgWidth*(-0.5f-x), -0.5f*avgHeight, z),
		glm::vec3(avgWidth*(0.5f+x), -0.5f*avgHeight, z),
		glm::vec3(avgWidth*(0.5f-x), 0.5f*avgHeight, z),
		glm::vec3(avgWidth*(-0.5f+x), 0.5f*avgHeight, z)
	};
	return myPolygon(poly, center);
}

myPolygon PolygonBuilder::nGon(unsigned int nPoints, float z,  float radius, float dPhi, glm::vec3 center) {
	if (nPoints < 3) {
		std::cout << "NGON must be more than 2 points!" << std::endl;
	}

	std::vector< glm::vec3 > poly;
	for (unsigned int i = 0; i < nPoints; i++) {
		float phi = i * 2 * 3.141592 / nPoints +dPhi;
		poly.push_back(glm::vec3(radius*cos(phi), radius*sin(phi), z));
	}
	return myPolygon(poly, center);
}

myPolygon PolygonBuilder::Sector(unsigned int nPoints, float z, float radius, float dPhi, glm::vec3 center) {
	if (nPoints < 2) {
		std::cout << "Sector must be more than 1 points!" << std::endl;
	}

	std::vector< glm::vec3 > poly;
	poly.push_back(glm::vec3(0.0f,0.0f,z));
	for (unsigned int i = 0; i < nPoints; i++) {
		float phi = i * dPhi / nPoints + dPhi;
		poly.push_back(glm::vec3(radius*cos(phi), radius*sin(phi), z));
	}
	return myPolygon(poly, center);
}

//poly1 and poly2 must have same number of points
std::vector< myPolygon > PolygonBuilder::tube2D(myPolygon poly1, float z1, myPolygon poly2, float z2, bool flipFaceOrientation){
	glm::vec3 c = glm::vec3(0);

	std::vector< myPolygon > polygons;

	//take sides of poly1 and connect them to points on poly2
	for (size_t j = 0; j < poly1.vtxs.size(); j++) {
		std::vector< glm::vec3 > p;
		glm::vec3 v1 = poly1.vtxs[j];
		v1.z = z1;
		p.push_back(v1);
		glm::vec3 v2 = poly1.vtxs[(j + 1 == poly1.vtxs.size()) ? 0 : j + 1];//handles the wrap around case
		v2.z = z1;
		p.push_back(v2);
		glm::vec3 v3 = poly2.vtxs[j];
		v3.z = z2;
		p.push_back(v3);
		if(!flipFaceOrientation) polygons.push_back(myPolygon(p, c));
		else polygons.push_back(myPolygon(p, p[0]+(p[0]-c)));
	}
	//same for poly2
	for (size_t j = 0; j < poly2.vtxs.size(); j++) {
		std::vector< glm::vec3 > p;
		glm::vec3 v1 = poly2.vtxs[j];
		v1.z = z2;
		p.push_back(v1);
		glm::vec3 v2 = poly2.vtxs[(j + 1 == poly2.vtxs.size()) ? 0 : j + 1];//handles the wrap around case
		v2.z = z2;
		p.push_back(v2);
		glm::vec3 v3 = poly1.vtxs[(j + 1 == poly1.vtxs.size()) ? 0 : j + 1];
		v3.z = z1;
		p.push_back(v3);
		if (!flipFaceOrientation) polygons.push_back(myPolygon(p, c));
		else polygons.push_back(myPolygon(p, p[0] + (p[0] - c)));
	}
	return polygons;
};

std::vector< myPolygon > PolygonBuilder::tube2D(myPolygon poly1, float z1, myPolygon poly2, float z2, unsigned int nInterpolations, bool flipFaceOrientation) {
	glm::vec3 c = glm::vec3(0);
	float dZ = z1 - z2;

	std::vector< myPolygon > polygons;

	myPolygon lastPoly = poly1;
	for (int i = 0; i < nInterpolations + 1; i++) {
		std::vector< glm::vec3 > interpolatedPolyPts;
		float frac1 = 1 - (i + 1) / (float)(nInterpolations + 1);
		for (size_t j = 0; j < poly1.vtxs.size(); j++) {
			float interpolatedX = (frac1)* poly1.vtxs[j].x + (1 - frac1) * poly2.vtxs[j].x;
			float interpolatedY = (frac1)* poly1.vtxs[j].y + (1 - frac1) * poly2.vtxs[j].y;

			interpolatedPolyPts.push_back(glm::vec3(interpolatedX, interpolatedY, 0.0f));
		}
		myPolygon interpolatedPoly = myPolygon(interpolatedPolyPts, glm::vec3(0, 0, 0));

		std::vector< myPolygon > tempPoly = tube2D(lastPoly, z1 - dZ*i / (float)(nInterpolations + 1), interpolatedPoly, z1 - dZ*(i + 1) / (float)(nInterpolations + 1), flipFaceOrientation);
		polygons.insert(std::end(polygons), std::begin(tempPoly), std::end(tempPoly));

		lastPoly = interpolatedPoly;
	}

	return polygons;
};

