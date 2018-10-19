#pragma once
#include "glm/glm.hpp"

enum matEnum {
	BRASS,
	BRONZE,
	CHROME,
	COPPER,
	GOLD,
	SILVER,
	EMERALD,
	JADE,
	OBSIDIAN,
	PEARL,
	RUBY,
	TURQUOISE,
	RED_PLASTIC,
	GREEN_PLASTIC,
	CYAN_PLASTIC,
	BLACK_PLASTIC,
	WHITE_PLASTIC,
	YELLOW_PLASTIC
};

class Material {
public:
	Material();
	Material(matEnum m);
	glm::vec3 getAmbient() { return ambient; }
	glm::vec3 getDiffuse() { return diffuse; }
	glm::vec3 getSpecular() { return specular; }
	float getShininess() { return shininess; }

	matEnum getMatEnum() { return thisMat; };

private:
	matEnum thisMat;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};
