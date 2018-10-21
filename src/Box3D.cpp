#include "include/Geometry/Box3D.h"

Box3D::Box3D(float x, float y, float z, float Lx, float Ly, float Lz) {
	boxBufferLayout.Push<float>(3);
	boxBufferLayout.Push<float>(3);

	glm::vec3 c = glm::vec3(0);//center of object

	std::vector< glm::vec3 > side[6];
	side[0] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f, 0.5f, -0.5f)
	};
	polygons.push_back(Polygon(side[0], c));

	side[1] = {
		glm::vec3(-0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f,  0.5f)
	};
	polygons.push_back(Polygon(side[1], c));

	side[2] = {
		glm::vec3(-0.5f,  0.5f,  0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f)
	};
	polygons.push_back(Polygon(side[2], c));

	side[3] = {
		glm::vec3(0.5f,  0.5f,  0.5f),
		glm::vec3(0.5f,  0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f)
	};
	polygons.push_back(Polygon(side[3], c));

	side[4] = {
		glm::vec3(-0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(0.5f, -0.5f,  0.5f),
		glm::vec3(-0.5f, -0.5f,  0.5f)
	};
	polygons.push_back(Polygon(side[4], c));

	side[5] = {
		glm::vec3(-0.5f, 0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(-0.5f, 0.5f, 0.5f)
	};
	polygons.push_back(Polygon(side[5], c));

	std::vector< float > positions;
	for (unsigned int i = 0; i < polygons.size(); i++) polygons[i].addBuffer(&positions, true);

boxVertexBuffer = new VertexBuffer(positions.data(), positions.size() * sizeof(float));
boxVertexArray.AddBuffer(*boxVertexBuffer, boxBufferLayout);

//uniforms
setOffXYZ(x, y, z);
setDimXYZ(Lx, Ly, Lz);
setRotXYZ(0, 0, 0);

//material
boxMaterial = Material(matEnum::RED_PLASTIC);
}

Box3D::~Box3D() {
	delete boxVertexBuffer;
}

void Box3D::Draw(Renderer* r, Shader* s)
{
	//face culling
	GLCall(glEnable(GL_CULL_FACE));

	r->DrawNoIB(boxVertexArray, *s, GL_TRIANGLES, 36);

	GLCall(glDisable(GL_CULL_FACE));
}

void Box3D::setUniforms(Shader* s) {
	s->Bind();
	s->SetUniform4x4f("u_Scale", scalingMatrix);
	s->SetUniform4x4f("u_Rotation", rotationMatrix);
	s->SetUniform4x4f("u_Translate", offsetMatrix);
	s->SetUniform3x3f("u_NormalMatrix", normalMatrix);

	s->SetUniform3f("u_light.ambient", 0.3f, 0.3f, 0.3f);
	s->SetUniform3f("u_light.diffuse", 0.5f, 0.5f, 0.5f);
	s->SetUniform3f("u_light.uniformLight", 0.2f, 0.2f, 0.2f);
	s->SetUniform3f("u_light.specular", 1.0f, 1.0f, 1.0f);

	s->SetUniform1f("u_material.shininess", boxMaterial.getShininess());
	s->SetUniform3fv("u_material.ambient", boxMaterial.getAmbient());
	s->SetUniform3fv("u_material.diffuse", boxMaterial.getDiffuse());
	s->SetUniform3fv("u_material.specular", boxMaterial.getSpecular());

}

void Box3D::updateOffsetMatrix() {
	offsetMatrix = glm::translate(glm::mat4(1.0), glm::vec3(offset[0], offset[1], offset[2]));
	offsetMatrixInv = glm::translate(glm::mat4(1.0), glm::vec3(-offset[0], -offset[1], -offset[2]));
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateNormalMatrix();
}

void Box3D::updateScalingMatrix() {
	scalingMatrix = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	scalingMatrixInv = glm::scale(glm::vec3(1.0 / scale[0], 1.0 / scale[1], 1.0 / scale[2]));
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateNormalMatrix();
}

void Box3D::updateRotationMatrix() {
	glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
	rotationMatrix = glm::rotate(rotation[2], z)*glm::rotate(rotation[1], y)*glm::rotate(rotation[0], x);
	rotationMatrixInv = glm::rotate(-rotation[0], x)*glm::rotate(-rotation[1], y)*glm::rotate(-rotation[2], z);
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateNormalMatrix();
}

void Box3D::updateNormalMatrix() {
	normalMatrix = glm::mat3(glm::transpose(glm::inverse(offsetMatrix*rotationMatrix*scalingMatrix)));
}

int Box3D::isInside(float x, float y, float z, float R, std::vector< Polygon >* polys, glm::mat4 preTransform) {
	return isInside(glm::vec3(x, y, z), R, polys, preTransform);
}

//checks if any part of a sphere of radius R is inside the sphere containing the object
//R=0 corresponds to checking if a point is inside the sphere containing the object
//if one supplies a preTransform matrix, the points in the points vector are transformed by that first before checking
//this allows one to supply points in model coordinates of 1 model, transform to world coordinates and back to model coordinates of object 2, where checking overlaps is easy

//returns 0 if not inside, 1 if IS inside, -1 is undecided
int Box3D::isInside(glm::vec3 v, float R, std::vector< Polygon >* polys, glm::mat4 preTransform) {
	//do quick check with sphere radius before doing matrix transform
	//can only determine if the two bounding spheres don't touch
	if (glm::distance(v, sphereCenter) > sphereRadius + R) return 0;

	//do a more indepth check of the point given
	glm::vec3 transformedV = inverseMatrix*glm::vec4(v, 1.0);
	if (insideBounds(transformedV)) {
		if (fabs(R) < 0.00001f) return 1;//if we were just checking the point, it is not inside, othersise we are still not sure
	}

	//if we got a list of polygons, check all those rigorously
	//this either returns 1 or 0 because it checks everything
	if (polys != NULL) {
		glm::mat4 netTransform = inverseMatrix*preTransform;
		//loop over polygons
		for (size_t i = 0; i < polys->size(); ++i) {
			Polygon poly = polys->at(i);

			//quick check at polygon level before checking all triangles
			if (glm::distance(poly.getCollisionSphereCenter(), sphereCenter) > sphereRadius + poly.getScaledCollisionSphereRadius(sphereCenter)) continue;

			//loop over triangles making up the polygon
			for (size_t j = 0; j < poly.polygonTriangles.size(); j++){

				//loop over points in the triangle
				for (unsigned int k = 0; k < 3; k++) {
					glm::vec3 transformedV = netTransform*glm::vec4(poly.polygonTriangles[j].getPoint(k), 1.0);
					if (insideBounds(transformedV)) return 1;
				}
			}
		}
		return 0;
	}
	//finished with rigorous check

	return -1;
}

bool Box3D::insideBounds(glm::vec3 v) {
	return fabs(v.x) < 0.5 && fabs(v.y) < 0.5 && fabs(v.z) < 0.5;
}

void Box3D::updateSphereCenter() {
	sphereCenter = glm::vec3(offset[0], offset[1], offset[2]);
}

void Box3D::updateSphereRadius() {
	sphereRadius = (float)pow(scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2], 0.5) / 2.0;
}

void Box3D::setDimX(float f) {
	scale[0] = f; 
	updateSphereRadius();
	updateScalingMatrix();
}
void Box3D::setDimY(float f) {
	scale[1] = f;
	updateSphereRadius();
	updateScalingMatrix();
}
void Box3D::setDimZ(float f) {
	scale[2] = f; 
	updateSphereRadius();
	updateScalingMatrix();
}
void Box3D::setDimXY(float f1, float f2) {
	scale[0] = f1;
	scale[1] = f2; 
	updateSphereRadius();
	updateScalingMatrix();
}
void Box3D::setDimXYZ(float f1, float f2, float f3) {
	scale[0] = f1; 
	scale[1] = f2; 
	scale[2] = f3; 
	updateSphereRadius();
	updateScalingMatrix();
}

void Box3D::setOffX(float f) {
	offset[0] = f; 
	updateSphereCenter();
	updateOffsetMatrix();
}
void Box3D::setOffY(float f) {
	offset[1] = f; 
	updateSphereCenter();
	updateOffsetMatrix();
}
void Box3D::setOffZ(float f) {
	offset[2] = f; 
	updateSphereCenter();
	updateOffsetMatrix();
}
void Box3D::setOffXY(float f1, float f2) {
	offset[0] = f1; 
	offset[1] = f2; 
	updateSphereCenter();
	updateOffsetMatrix();
}
void Box3D::setOffXYZ(float f1, float f2, float f3) {
	offset[0] = f1; 
	offset[1] = f2; 
	offset[2] = f3; 
	updateSphereCenter();
	updateOffsetMatrix();
}

void Box3D::setRotX(float f) {
	rotation[0] = f; 
	updateRotationMatrix();
}
void Box3D::setRotY(float f) {
	rotation[1] = f; 
	updateRotationMatrix();
}
void Box3D::setRotZ(float f) {
	rotation[2] = f; 
	updateRotationMatrix();
}
void Box3D::setRotXY(float f1, float f2) {
	rotation[0] = f1; 
	rotation[1] = f2; 
	updateRotationMatrix();
}
void Box3D::setRotXYZ(float f1, float f2, float f3) {
	rotation[0] = f1; 
	rotation[1] = f2; 
	rotation[2] = f3; 
	updateRotationMatrix();
}