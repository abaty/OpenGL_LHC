#pragma once
#include "glm/glm.hpp"
#include "include/Geometry/Materials.h"
#include "include/VertexArray.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include "include/Renderer.h"
#include "include/shader.h"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

class Box3D {

public:
	Box3D(float x = 0, float y = 0, float z = 0, float Lx = 1, float Ly = 1, float Lz = 1);
	~Box3D();
	void Draw(Renderer* r, Shader* s);
	void setUniforms(Shader * s);

	//functions for setting dimensions
	void setDimX(float f);
	void setDimY(float f);
	void setDimZ(float f);
	void setDimXY(float f1, float f2);
	void setDimXYZ(float f1, float f2, float f3);
	float getDimX() { return scale[0]; };
	float getDimY() { return scale[1]; };
	float getDimZ() { return scale[2]; };

	//functions for setting offset of box
	void setOffX(float f);
	void setOffY(float f);
	void setOffZ(float f);
	void setOffXY(float f1, float f2);
	void setOffXYZ(float f1, float f2, float f3);
	float getOffX() { return offset[0]; };
	float getOffY() { return offset[1]; };
	float getOffZ() { return offset[2]; };

	//functions for setting rotation of box
	void setRotX(float f);
	void setRotY(float f);
	void setRotZ(float f);
	void setRotXY(float f1, float f2);
	void setRotXYZ(float f1, float f2, float f3);
	float getRotX() { return rotation[0]; };
	float getRotY() { return rotation[1]; };
	float getRotZ() { return rotation[2]; };

	void setMaterial(matEnum m) { boxMaterial = Material(m); };
	matEnum getMaterial() { return boxMaterial.getMatEnum(); };

	//collision detection
	bool isInside(float x, float y, float z, float R = 0, std::vector< glm::vec3 >* points = NULL, glm::mat4 preTransform = glm::mat4(1.0));
	bool isInside(glm::vec3 v, float R = 0, std::vector< glm::vec3 >* points = NULL, glm::mat4 preTransform = glm::mat4(1.0));
	float getCollisionSphereRadius() { return sphereRadius; };
	glm::vec3 getCollisionSphereCenter() { return sphereCenter; };

	//rendering items
	VertexBufferLayout boxBufferLayout;
	VertexArray boxVertexArray;
	VertexBuffer *boxVertexBuffer;

private:

	glm::vec3 color;

	//things for offset matrix
	float offset[3] = { 0.0f, 0.0f, 0.0f };
	glm::mat4 offsetMatrix;
	glm::mat4 offsetMatrixInv;
	void updateOffsetMatrix();

	//things for scaling matrix
	float scale[3] = { 1.0, 1.0, 1.0 };
	glm::mat4 scalingMatrix;
	glm::mat4 scalingMatrixInv;
	void updateScalingMatrix();

	//things for rotation matrix
	float rotation[3] = { 0.0, 0.0, 0.0 };
	glm::mat4 rotationMatrix;
	glm::mat4 rotationMatrixInv;
	void updateRotationMatrix();

	//inverse matrix
	glm::mat4 inverseMatrix;

	//normal matrix for transforming normals to world coordinates
	glm::mat3 normalMatrix;
	void updateNormalMatrix();

	//stuff for collision detection
	//define a sphere that contains all the points of the object
	glm::vec3 sphereCenter;
	float sphereRadius;
	void updateSphereRadius();
	void updateSphereCenter();

	//material
	Material boxMaterial;

	//helper function
	bool insideBounds(glm::vec3);
};