#pragma once
#include "include/Camera.h"
#include "include/Renderer.h"
#include "include/VertexArray.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "glm/glm.hpp"
#include <vector>
#include <iostream>

enum viewMode {
	ONE_SCREEN, 
	ONE_LEFT_TWO_SQUARES_RIGHT, 
	ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT, 
	ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM, 
	FOUR_CORNERS,
};

class MultiCamera {
public:
	MultiCamera(unsigned int _width, unsigned int _height);
	MultiCamera(unsigned int _width, unsigned int _height, glm::vec3 initialPosition);
	~MultiCamera();

	void Reset();

	std::vector< Camera > cameras;

	void setViewport(bool fullScreen, unsigned int cameraNumber);

	void setFieldOfView(float FoV);
	void setAspectRatio(float AR);
	void setNearVisionLimit(float NVL);
	void setFarVisionLimit(float FVL);

	void ZoomOut(float zoomFactor);
	void ZoomIn(float zoomFactor);//just calls ZoomOut with 1/zoomFactor

	void setViewMode(viewMode _mode);
	inline viewMode GetViewMode() { return mode; }
	int getNCameras();

	void DrawBorders(Renderer* r, Shader* s, unsigned int frameIndex);

	VertexBuffer* vertexBuffer;
	VertexArray* vertexArray;
	VertexBufferLayout layout;

private:
	std::vector< float > borderBuffer;
	std::vector< std::pair< int, int > > borderOffsetsAndLengths;

	glm::vec3 defaultCamera1;

	unsigned int width;
	unsigned int height;

	viewMode mode;
	void UpdateBorderBuffers();


	float fieldOfView;
	float aspectRatio;
	float nearVisionLimit;
	float farVisionLimit;
};