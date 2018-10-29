#include "include/Geometry/Object3DBase.h"

Object3DBase::Object3DBase(){
	boxBufferLayout.Push<float>(3);//vertex positions
	boxBufferLayout.Push<float>(3);//normals

	//material
	setMaterial(matEnum::OBSIDIAN);
}

bool Object3DBase::isInside(float x, float y, float z) {
	return isInside(glm::vec3(x, y, z));
}

bool Object3DBase::isInside(glm::vec3 v) {
	//first check if the point is in the AABB
	//if it is not, then return false
	if (!aabb.isInside(v)) return false;

	if (isHollow && isInsideHollowRegion(v)) return false;


	//if it is inside the AABB we need to be more careful:
	//check to see if the vector from the point is along the normal of the polygon
	//THIS ASSUMES A CONVEX POLYHEDRON unless isHollow is specified, (see tube3d for example of that case)
	glm::vec3 vPrime = inverseMatrix * glm::vec4(v, 1.0f);
	for (size_t i = 0; i < polygons.size(); i++) {
		//only take into account polygons with unflipped normals (for tubes and hollow stuff that have surfaces facing the center,
		//we ignore inside surfaces)
		if (polygons[i].getIsNormalFlipped()) continue;

		//put the point into model coordinates
		if (glm::dot(polygons[i].getNormal(), polygons[i].vtxs[0] - vPrime) < 0) return false;
	}
	return true;
}

bool Object3DBase::isInside(Object3DBase *obj) {
	//first check if the other objects aabb is in this AABB
	//if it is not, then return false
	if (!this->aabb.isInside(obj->aabb)) return false;

	//check if polygons are in the same plane
    //following http://web.stanford.edu/class/cs277/resources/papers/Moller1997b.pdf
	for (size_t i = 0; i < polygons.size(); i++) {
		//put the point into model coordinates
		//step 1 in paper
		glm::vec3 polyPoint = transformationMatrix * glm::vec4(polygons[i].vtxs[0],1.0);
		glm::vec3 polyNorm = normalMatrix * polygons[i].getNormal();
		float d2 = -glm::dot(polyNorm, polyPoint);
		
		//loop over other points on the polygons
		//step 2 in paper
		for (size_t j = 0; j < obj->polygons.size(); j++) {
			bool areOnPlusSide = false;
			bool areOnMinusSide = false;
			std::vector< float > distancesObjPoly;
			std::vector< glm::vec3 > polyPointObj;
			for (size_t k = 0; k < obj->polygons[j].vtxs.size(); k++) {
				polyPointObj.push_back( glm::vec3(obj->transformationMatrix * glm::vec4(obj->polygons[j].vtxs[k], 1.0)));
				float dist = glm::dot(polyNorm, polyPointObj[k]) + d2;
				distancesObjPoly.push_back(dist);
				bool isPositive = ( dist >= 0) ? true : false ;

				if (isPositive) areOnPlusSide = true;
				else areOnMinusSide = true;
			}

			//after loop, if all on same side then we continue
			if (!(areOnPlusSide && areOnMinusSide)) continue;

			//repeat same procedure as above but using plane of 2nd polygon and checking points of first polygon
			//step 3 in paper
			glm::vec3 poly2Point = obj->transformationMatrix * glm::vec4(obj->polygons[j].vtxs[0], 1.0);
			glm::vec3 poly2Norm = obj->normalMatrix * obj->polygons[j].getNormal();
			float d2_2 = -glm::dot(poly2Norm, poly2Point);

			//step 4 in paper
			//looping over points in polygon i now
			areOnPlusSide = false;
			areOnMinusSide = false;
			std::vector< float > distancesThisPoly;
			std::vector< glm::vec3 > polyPointThis;
			for (size_t k = 0; k < polygons[i].vtxs.size(); k++) {
				polyPointThis.push_back( glm::vec3( transformationMatrix * glm::vec4(polygons[i].vtxs[k], 1.0)));
				float dist = glm::dot(poly2Norm, polyPointThis[k]) + d2_2;
				distancesThisPoly.push_back(dist);
				bool isPositive = ( dist >= 0) ? true : false;

				if (isPositive) areOnPlusSide = true;
				else areOnMinusSide = true;
			}
			//after loop, if all on same side then we continue
			if (!(areOnPlusSide && areOnMinusSide)) continue;

			//std::cout << "We got to here for " << i << " " << j << std::endl;

			//if we get to here, then polygons i and j have a line that is in both of their planes
			//we need to check if their projection onto this line overlaps
			
			//step 5 in paper
			glm::vec3 projectionLine = glm::cross(polyNorm, poly2Norm);
			//choose an axis to project onto (optimization in paper)
			//a=0:x a=1:y a=0:z
			unsigned char a = 0;
			if (fabs(projectionLine.y) > fabs(projectionLine.x) && fabs(projectionLine.y) > fabs(projectionLine.z)) a = 1;
			if (fabs(projectionLine.z) > fabs(projectionLine.x) && fabs(projectionLine.z) > fabs(projectionLine.y)) a = 2;

			//step 6 (calculate interval for this polygon)
			bool foundIntervalAlready = false;
			float t1Min = 0;
			float t1Max = 0;
			for (size_t k = 0; k < polygons[i].vtxs.size(); k++) {
				size_t nextIndex = (k == polygons[i].vtxs.size() - 1) ? 0 : k + 1;//wrap around if needed

				//if they have the same sign as the next point (doesn't cross intersection), continue
				if (distancesThisPoly[k] * distancesThisPoly[nextIndex] >= 0) continue;

				float distRatio = distancesThisPoly[k]/(distancesThisPoly[k] - distancesThisPoly[nextIndex]);
				float t = polyPointThis[k][a] + (polyPointThis[nextIndex][a] - polyPointThis[k][a]) * distRatio;

				if (!foundIntervalAlready) {
					t1Min = t;
					t1Max = t;
					foundIntervalAlready = true;
				}
				else {
					if (t < t1Min)  t1Min = t;
					else if (t >= t1Max)  t1Max = t;
					break;
				}
			}

			//calculate interval for 2nd polygon
			foundIntervalAlready = false;
			float t2Min = 0;
			float t2Max = 0;
			for (size_t k = 0; k < obj->polygons[j].vtxs.size(); k++) {
				size_t nextIndex = (k == obj->polygons[j].vtxs.size() - 1) ? 0 : k + 1;//wrap around if needed

				//if they have the same sign as the next point (doesn't cross intersection), continue
				if (distancesObjPoly[k] * distancesObjPoly[nextIndex] >= 0) continue;

				float distRatio = distancesObjPoly[k] / (distancesObjPoly[k] - distancesObjPoly[nextIndex]);
				float t = polyPointObj[k][a] + (polyPointObj[nextIndex][a] - polyPointObj[k][a]) * distRatio;

				if (!foundIntervalAlready) {
					t2Min = t;
					t2Max = t;
					foundIntervalAlready = true;
				}
				else {
					if (t < t2Min)  t2Min = t;
					else if (t >= t2Max)  t2Max = t;
					break;
				}
			}

			//step 7, check if the intervals intersect
			//if they do, we have an intersection
			bool noIntersect = (t2Max <= t1Min) || (t2Min >= t1Max);
			if (!noIntersect) return true;
		}
	}
	return false;
}

bool Object3DBase::isInsideHollowRegion(glm::vec3 v) {
	return false;
}

void Object3DBase::updateAABB() {
	for (size_t i = 0; i < polygons.size(); ++i) {
		myPolygon poly = polygons.at(i);
		for (size_t j = 0; j < poly.vtxs.size(); j++) {
			glm::vec3 worldCoordinates = transformationMatrix * glm::vec4(poly.vtxs[j], 1.0f);
			//reset to first vertex first
			if (i == 0 && j == 0) aabb.reset(worldCoordinates);
			else aabb.update(worldCoordinates);
		}
	}
}

void Object3DBase::updateOffsetMatrix() {
	offsetMatrix = glm::translate(glm::mat4(1.0), glm::vec3(offset[0], offset[1], offset[2]));
	offsetMatrixInv = glm::translate(glm::mat4(1.0), glm::vec3(-offset[0], -offset[1], -offset[2]));
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateTransformationMatrix();
	updateNormalMatrix();
}

void Object3DBase::updateScalingMatrix() {
	scalingMatrix = glm::scale(glm::vec3(scale[0], scale[1], scale[2]));
	scalingMatrixInv = glm::scale(glm::vec3(1.0 / scale[0], 1.0 / scale[1], 1.0 / scale[2]));
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateTransformationMatrix();
	updateNormalMatrix();
}

void Object3DBase::updateRotationMatrix() {
	glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 y = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 z = glm::vec3(0.0f, 0.0f, 1.0f);
	rotationMatrix = glm::rotate(rotation[2], z)*glm::rotate(rotation[1], y)*glm::rotate(rotation[0], x);
	rotationMatrixInv = glm::rotate(-rotation[0], x)*glm::rotate(-rotation[1], y)*glm::rotate(-rotation[2], z);
	inverseMatrix = scalingMatrixInv*rotationMatrixInv*offsetMatrixInv;
	updateTransformationMatrix();
	updateNormalMatrix();
}

void Object3DBase::updateNormalMatrix() {
	normalMatrix = glm::mat3(glm::transpose(glm::inverse(transformationMatrix)));
}

void Object3DBase::updateTransformationMatrix() {
	transformationMatrix = offsetMatrix*rotationMatrix*scalingMatrix;
	updateAABB();
}

void Object3DBase::setDimX(float f) {
	scale[0] = f;
	updateScalingMatrix();
}
void Object3DBase::setDimY(float f) {
	scale[1] = f;
	updateScalingMatrix();
}
void Object3DBase::setDimZ(float f) {
	scale[2] = f;
	updateScalingMatrix();
}
void Object3DBase::setDimXY(float f1, float f2) {
	scale[0] = f1;
	scale[1] = f2;
	updateScalingMatrix();
}
void Object3DBase::setDimXYZ(float f1, float f2, float f3) {
	scale[0] = f1;
	scale[1] = f2;
	scale[2] = f3;
	updateScalingMatrix();
}

void Object3DBase::setOffX(float f) {
	offset[0] = f;
	updateOffsetMatrix();
}
void Object3DBase::setOffY(float f) {
	offset[1] = f;
	updateOffsetMatrix();
}
void Object3DBase::setOffZ(float f) {
	offset[2] = f;
	updateOffsetMatrix();
}
void Object3DBase::setOffXY(float f1, float f2) {
	offset[0] = f1;
	offset[1] = f2;
	updateOffsetMatrix();
}
void Object3DBase::setOffXYZ(float f1, float f2, float f3) {
	offset[0] = f1;
	offset[1] = f2;
	offset[2] = f3;
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