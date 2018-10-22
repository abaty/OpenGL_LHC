#include "include/Geometry/Object3DBase.h"

Object3DBase::Object3DBase(){
	boxBufferLayout.Push<float>(3);//vertex positions
	boxBufferLayout.Push<float>(3);//normals

	//material
	setMaterial(matEnum::RED_PLASTIC);
}

int Object3DBase::isInside(float x, float y, float z, float R, std::vector< myPolygon >* polys, glm::mat4 preTransform) {
	return isInside(glm::vec3(x, y, z), R, polys, preTransform);
}

//checks if any part of a sphere of radius R is inside the sphere containing the object
//R=0 corresponds to checking if a point is inside the sphere containing the object
//if one supplies a preTransform matrix, the points in the points vector are transformed by that first before checking
//this allows one to supply points in model coordinates of 1 model, transform to world coordinates and back to model coordinates of object 2, where checking overlaps is easy

//returns 0 if not inside, 1 if IS inside, -1 is undecided
int Object3DBase::isInside(glm::vec3 v, float R, std::vector< myPolygon >* polys, glm::mat4 preTransform) {
	//do quick check with sphere radius before doing matrix transform
	//can only determine if the two bounding spheres don't touch
	if (glm::distance(v, sphereCenter) > sphereRadius + R) return 0;

	//do a more indepth check of the point given
	glm::vec3 transformedV = getInverseMatrix()*glm::vec4(v, 1.0);
	if (insideBounds(transformedV)) {
		if (fabs(R) < 0.00001f) return 1;//if we were just checking the point, it is not inside, othersise we are still not sure
	}

	//if we got a list of polygons, check all those rigorously
	//this either returns 1 or 0 because it checks everything
	if (polys != NULL) {
		glm::mat4 netTransform = getInverseMatrix()*preTransform;
		//loop over polygons
		for (size_t i = 0; i < polys->size(); ++i) {
			myPolygon poly = polys->at(i);

			//quick check at polygon level before checking all triangles
			if (glm::distance(poly.getCollisionSphereCenter(), sphereCenter) > sphereRadius + poly.getScaledCollisionSphereRadius(sphereCenter)) continue;

			for (size_t j = 0; j < poly.vtxs.size(); j++) {
				glm::vec3 transformedV = netTransform*glm::vec4(poly.vtxs[j], 1.0);
				if (insideBounds(transformedV)) return 1;
			}
		}
		return 0;
	}
	//finished with rigorous check

	return -1;
}


void Object3DBase::updateSphereCenter() {
	sphereCenter = glm::vec3(offset[0], offset[1], offset[2]);
}

void Object3DBase::updateSphereRadius() {
	float R = 0;

	for (size_t i = 0; i < polygons.size(); ++i) {
		myPolygon poly = polygons.at(i);
		for (size_t j = 0; j < poly.vtxs.size(); j++) {
			float newR = glm::length(poly.vtxs[j]);
			if (newR > R) R = newR;
		}
	}
	sphereRadius = R;
}

void Object3DBase::updateOffsetMatrix() {
	offsetMatrix = glm::translate(glm::mat4(1.0), glm::vec3(offset[0], offset[1], offset[2]));
	offsetMatrixInv = glm::translate(glm::mat4(1.0), glm::vec3(-offset[0], -offset[1], -offset[2]));
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateNormalMatrix();
}

void Object3DBase::updateScalingMatrix() {
	scalingMatrix = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	scalingMatrixInv = glm::scale(glm::vec3(1.0 / scale[0], 1.0 / scale[1], 1.0 / scale[2]));
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateNormalMatrix();
}

void Object3DBase::updateRotationMatrix() {
	glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
	rotationMatrix = glm::rotate(rotation[2], z)*glm::rotate(rotation[1], y)*glm::rotate(rotation[0], x);
	rotationMatrixInv = glm::rotate(-rotation[0], x)*glm::rotate(-rotation[1], y)*glm::rotate(-rotation[2], z);
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateNormalMatrix();
}

void Object3DBase::updateNormalMatrix() {
	normalMatrix = glm::mat3(glm::transpose(glm::inverse(offsetMatrix*rotationMatrix*scalingMatrix)));
}


void Object3DBase::setDimX(float f) {
	scale[0] = f;
	updateSphereRadius();
	updateScalingMatrix();
}
void Object3DBase::setDimY(float f) {
	scale[1] = f;
	updateSphereRadius();
	updateScalingMatrix();
}
void Object3DBase::setDimZ(float f) {
	scale[2] = f;
	updateSphereRadius();
	updateScalingMatrix();
}
void Object3DBase::setDimXY(float f1, float f2) {
	scale[0] = f1;
	scale[1] = f2;
	updateSphereRadius();
	updateScalingMatrix();
}
void Object3DBase::setDimXYZ(float f1, float f2, float f3) {
	scale[0] = f1;
	scale[1] = f2;
	scale[2] = f3;
	updateSphereRadius();
	updateScalingMatrix();
}

void Object3DBase::setOffX(float f) {
	offset[0] = f;
	updateSphereCenter();
	updateOffsetMatrix();
}
void Object3DBase::setOffY(float f) {
	offset[1] = f;
	updateSphereCenter();
	updateOffsetMatrix();
}
void Object3DBase::setOffZ(float f) {
	offset[2] = f;
	updateSphereCenter();
	updateOffsetMatrix();
}
void Object3DBase::setOffXY(float f1, float f2) {
	offset[0] = f1;
	offset[1] = f2;
	updateSphereCenter();
	updateOffsetMatrix();
}
void Object3DBase::setOffXYZ(float f1, float f2, float f3) {
	offset[0] = f1;
	offset[1] = f2;
	offset[2] = f3;
	updateSphereCenter();
	updateOffsetMatrix();
}

void Object3DBase::setRotX(float f) {
	rotation[0] = f;
	updateRotationMatrix();
}
void Object3DBase::setRotY(float f) {
	rotation[1] = f;
	updateRotationMatrix();
}
void Object3DBase::setRotZ(float f) {
	rotation[2] = f;
	updateRotationMatrix();
}
void Object3DBase::setRotXY(float f1, float f2) {
	rotation[0] = f1;
	rotation[1] = f2;
	updateRotationMatrix();
}
void Object3DBase::setRotXYZ(float f1, float f2, float f3) {
	rotation[0] = f1;
	rotation[1] = f2;
	rotation[2] = f3;
	updateRotationMatrix();
}

bool Object3DBase::insideBounds(glm::vec3 v) {
	for (size_t i = 0; i < polygons.size(); i++){
		if (glm::dot(polygons[i].getNormal(), polygons[i].vtxs[0] - v) < 0) return false;
	}
	return true;
}


void Object3DBase::Draw(Renderer* r, Shader* s)
{
	//face culling
	GLCall(glEnable(GL_CULL_FACE));

	r->DrawNoIB(boxVertexArray, *s, GL_TRIANGLES, nVertices );

	GLCall(glDisable(GL_CULL_FACE));
}

void Object3DBase::setUniforms(Shader* s) {
	s->Bind();
	s->SetUniform4x4f("u_Scale", getScalingMatrix());
	s->SetUniform4x4f("u_Rotation", getRotationMatrix());
	s->SetUniform4x4f("u_Translate", getOffsetMatrix());
	s->SetUniform3x3f("u_NormalMatrix", getNormalMatrix());

	s->SetUniform3f("u_light.ambient", 0.3f, 0.3f, 0.3f);
	s->SetUniform3f("u_light.diffuse", 0.5f, 0.5f, 0.5f);
	s->SetUniform3f("u_light.uniformLight", 0.2f, 0.2f, 0.2f);
	s->SetUniform3f("u_light.specular", 1.0f, 1.0f, 1.0f);

	s->SetUniform1f("u_material.shininess", getMaterial().getShininess());
	s->SetUniform3fv("u_material.ambient", getMaterial().getAmbient());
	s->SetUniform3fv("u_material.diffuse", getMaterial().getDiffuse());
	s->SetUniform3fv("u_material.specular", getMaterial().getSpecular());
}