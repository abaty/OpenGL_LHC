#pragma once
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

class Camera {
private:
	glm::vec3 defaultCamera;

	glm::vec3 position;
	glm::vec3 centerOfWorld;

	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	float fieldOfView;
	float aspectRatio;
	float nearVisionLimit;
	float farVisionLimit;

	bool doShowBeampipe = true;

public:

	Camera();
	Camera(glm::vec3 _position);
	~Camera();

	void Rotate(float angle, glm::vec3 axis = glm::vec3(0.0, 1.0f, 0.0f));
	void ZoomOut(float zoomFactor);
	void ZoomIn(float zoomFactor);//just calls ZoomOut with 1/zoomFactor

	inline glm::vec3 getPosition() { return position; }
	inline glm::vec3 getCenterOfWorld() { return centerOfWorld; }
	float getDistanceFromCenterOfWorld();

	inline void setPosition(glm::vec3 _position) { position = _position; }
	inline void setCenterOfWorld(glm::vec3 _center) { centerOfWorld = _center; }

	void sideView();
	void frontView();
	void topView();
	void isometricView();
	void resetView();

	inline bool GetDoShowBeamPipe() { return doShowBeampipe; };

	inline glm::mat4 getViewMatrix() { return viewMatrix; }
	inline glm::mat4 getProjectionMatrix() { return projectionMatrix; }
	inline glm::mat4 getProjectionViewMatrix() { return projectionMatrix * viewMatrix; }

	void setFieldOfView(float FoV);
	void setAspectRatio(float AR);
	void setNearVisionLimit(float NVL);
	void setFarVisionLimit(float FVL);

	inline float getAspectRatio() { return aspectRatio; }
};