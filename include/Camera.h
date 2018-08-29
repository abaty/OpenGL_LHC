#pragma once
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

class Camera {
private:

	glm::vec3 position;
	glm::vec3 centerOfWorld;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	float fieldOfView;
	float aspectRatio;
	float nearVisionLimit;
	float farVisionLimit;

public:

	Camera();
	Camera(glm::vec3 _position);
	~Camera();

	void Rotate(float angle, glm::vec3 axis = glm::vec3(0.0, 1.0f, 0.0f));

	inline glm::vec3 getPosition() { return position; }
	inline glm::vec3 getCenterOfWorld() { return centerOfWorld; }

	inline void setPosition(glm::vec3 _position) { position = _position; }
	inline void setCenterOfWorld(glm::vec3 _center) { centerOfWorld = _center; }

	void sideView();
	void frontView();
	void topView();
	void isometricView();

	inline glm::mat4 getViewMatrix() { return viewMatrix; }
	inline glm::mat4 getProjectionMatrix() { return projectionMatrix; }
	inline glm::mat4 getProjectionViewMatrix() { return projectionMatrix * viewMatrix; }

	inline void setFieldOfView(float FoV) { fieldOfView = FoV; }
	inline void setAspectRatio(float AR) { aspectRatio = AR; }
	inline void setNearVisionLimit(float NVL) { nearVisionLimit = NVL; }
	inline void setFarVisionLimit(float FVL) { farVisionLimit = FVL; }
};