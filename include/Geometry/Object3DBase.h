#pragma once

#include "glm/glm.hpp"
#include "include/Geometry/AABB.h"
#include "include/Geometry/Materials.h"
#include "include/Geometry/myPolygon.h"
#include "include/VertexArray.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include "include/Renderer.h"
#include "include/shader.h"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

//base class for 3d objects
//things can be overwritten in specific classes by making functions virtual as needed

//if you are not dealing with convex polyhedra, isInside() might have to be modified in particular

class Object3DBase {
	public:
		Object3DBase();

		//drawing stuff
		void Draw(Renderer* r, Shader* s);
		void setUniforms(Shader * s);

		//collision detection stuff
		AABB aabb;
		bool isInside(float x, float y, float z);
		bool isInside(glm::vec3 v);
		bool isInside(Object3DBase *obj);

		//stuff for hollow objects (tubes, etc)
		bool isHollow = false;
		virtual bool isInsideHollowRegion(glm::vec3 v);

		//functions for setting dimensions
		void setDimX(float f);
		void setDimY(float f);
		void setDimZ(float f);
		void setDimXY(float f1, float f2);
		void setDimXYZ(float f1, float f2, float f3);
		float getDimX() { return scale[0]; };
		float getDimY() { return scale[1]; };
		float getDimZ() { return scale[2]; };
		glm::mat4 getScalingMatrix() { return scalingMatrix; };

		//functions for setting offset of box
		void setOffX(float f);
		void setOffY(float f);
		void setOffZ(float f);
		void setOffXY(float f1, float f2);
		void setOffXYZ(float f1, float f2, float f3);
		float getOffX() { return offset[0]; };
		float getOffY() { return offset[1]; };
		float getOffZ() { return offset[2]; };
		glm::mat4 getOffsetMatrix() { return offsetMatrix; };

		//functions for setting rotation of box
		void setRotX(float f);
		void setRotY(float f);
		void setRotZ(float f);
		void setRotXY(float f1, float f2);
		void setRotXYZ(float f1, float f2, float f3);
		float getRotX() { return rotation[0]; };
		float getRotY() { return rotation[1]; };
		float getRotZ() { return rotation[2]; };
		glm::mat4 getRotationMatrix() { return rotationMatrix; };

		//other matrices
		glm::mat4 getTransformationMatrix() { return transformationMatrix;  };
		glm::mat4 getNormalMatrix() { return normalMatrix; };
		glm::mat4 getInverseMatrix() { return inverseMatrix; };

		//material
		void setMaterial(matEnum m) { boxMaterial = Material(m); };
		Material getMaterial() { return boxMaterial; };

		//list of polygons in the object
		std::vector< myPolygon > polygons;

		//rendering buffer items
		void setNVertices(unsigned int n) { nVertices = n; };
		unsigned int getNVertices() { return nVertices; };
		VertexBufferLayout boxBufferLayout;
		VertexArray boxVertexArray;
		VertexBuffer *boxVertexBuffer;

	private:
		unsigned int nVertices;

		//collision detection
		void updateAABB();

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

		glm::mat4 transformationMatrix;
		void updateTransformationMatrix();

		//material
		Material boxMaterial;
};