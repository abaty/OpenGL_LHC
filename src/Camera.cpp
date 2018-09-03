#include "include/Camera.h"

Camera::Camera() 
	:fieldOfView(45.0f), aspectRatio(4.0f/3.0f), nearVisionLimit(0.01f), farVisionLimit(100.0f)
{
	//no transformation (camera at origin)
	position = glm::vec3(0, 0, 0.01);
	centerOfWorld = glm::vec3(0, 0, 0);
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
	projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearVisionLimit, farVisionLimit);
}

Camera::Camera(glm::vec3 _position) 
	:fieldOfView(45.0f), aspectRatio(4.0f/ 3.0f), nearVisionLimit(0.01f), farVisionLimit(100.0f)
{
	position = _position;
	centerOfWorld = glm::vec3(0, 0, 0);
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
	projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearVisionLimit, farVisionLimit);
}

Camera::~Camera() {

}

void Camera::Rotate(float angle, glm::vec3 axis) {
	position = centerOfWorld + (glm::vec3)(glm::rotate(angle, axis) * glm::vec4(position - centerOfWorld, 0.0));
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
}

void Camera::ZoomOut(float zoomFactor) {
	position = position + (zoomFactor-1.0f)*(position-centerOfWorld);
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
}
void Camera::ZoomIn(float zoomFactor) {
	ZoomOut(1.0f / zoomFactor);
}

void Camera::sideView() {
	setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
	setCenterOfWorld(glm::vec3(0.0f, 0.0f, 0.0f));
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
}

void Camera::frontView() {
	setPosition(glm::vec3(10.0f, 0.0f, 0.0f));
	setCenterOfWorld(glm::vec3(0.0f, 0.0f, 0.0f));
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
}

void Camera::topView() {
	setPosition(glm::vec3(0.01f, 10.0f, 0.0f));
	setCenterOfWorld(glm::vec3(0.0f, 0.0f, 0.0f));
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
}

void Camera::isometricView() {
	setPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	setCenterOfWorld(glm::vec3(0.0f, 0.0f, 0.0f));
	viewMatrix = glm::lookAt(position, centerOfWorld, glm::vec3(0, 1, 0));
}

float Camera::getDistanceFromCenterOfWorld() {
	return glm::length(position - centerOfWorld);
}