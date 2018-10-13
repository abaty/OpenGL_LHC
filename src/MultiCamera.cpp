#include "include/MultiCamera.h"

MultiCamera::MultiCamera(unsigned int _width, unsigned int _height) {
	MultiCamera(_width, _height, glm::vec3(0.0f, 3.0f, 10.0f));
}

MultiCamera::MultiCamera(unsigned int _width, unsigned int _height, glm::vec3 initialPosition){
	width = _width;
	height = _height;

	layout.Push<float>(2);
	vertexBuffer = new VertexBuffer();
	vertexArray = new VertexArray();

	defaultCamera1 = initialPosition;

	for (int i = 0; i < 4; i++) {
		Camera c = Camera(initialPosition);
		cameras.push_back(c);
	}
	setAspectRatio((float)width / (float)height);
	Reset();
	mode = viewMode::ONE_SCREEN;
}

void MultiCamera::setViewport(bool fullScreen, unsigned int cameraNumber) {
	if (mode == viewMode::ONE_SCREEN || fullScreen) {
		GLCall(glViewport(0, 0, width, height));
		return;
	}
	
	if (mode == viewMode::FOUR_CORNERS) {
		if (cameraNumber > getNCameras()) {
			std::cout << "Camera index is greater than the number of cameras in this mode!  Setting 1 view port!" << std::endl;
			GLCall(glViewport(0, 0, width, height));
		}
		if (cameraNumber == 0) {
			GLCall(glViewport(0, height/2, width/2, height/2));
		}
		if (cameraNumber == 1) {
			GLCall(glViewport(width/2, height/2, width / 2, height / 2));
		}
		if (cameraNumber == 2) {
			GLCall(glViewport(0, 0, width / 2, height / 2));
		}
		if (cameraNumber == 3) {
			GLCall(glViewport(width / 2, 0, width / 2, height / 2));
		}
		return;
	}

	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT) {
		if (cameraNumber > getNCameras()) {
			std::cout << "Camera index is greater than the number of cameras in this mode!  Setting 1 view port!" << std::endl;
			GLCall(glViewport(0, 0, width, height));
		}
		if (cameraNumber == 0) {
			GLCall(glViewport(0, 0, width - height/2, height));
		}
		if (cameraNumber == 1) {
			GLCall(glViewport(width - height / 2, height / 2, height / 2, height / 2));
		}
		if (cameraNumber == 2) {
			GLCall(glViewport(width - height / 2, 0, height / 2, height / 2));
		}
		return;
	}
	
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT || mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM) {
		if (cameraNumber > getNCameras()) {
			std::cout << "Camera index is greater than the number of cameras in this mode!  Setting 1 view port!" << std::endl;
			GLCall(glViewport(0, 0, width, height));
		}
		if (cameraNumber == 0) {
			GLCall(glViewport(0, 0, width - height / 2, height));
		}
		if (cameraNumber == 1) {
			GLCall(glViewport(width - height / 2, height / 2, height / 2, height / 2));
		}
		if (cameraNumber == 2) {
			GLCall(glViewport(width - height / 2, height / 4, height / 2, height / 4));
		}
		if (cameraNumber == 3) {
			GLCall(glViewport(width - height / 2, 0, height / 2, height / 4));
		}
		return;
	}

}

void MultiCamera::Reset() {
	for (int i = 0; i < 4; i++) {
		if (i == 0) cameras.at(i) = Camera(defaultCamera1);
		if (i == 1) cameras.at(i).frontView();
		if (i == 2) cameras.at(i).sideView();
		if (i == 3) cameras.at(i).topView();
	}
}

void MultiCamera::ZoomOut(float zoomFactor) {
	for (int i = 0; i < 4; i++) {
		cameras.at(i).ZoomOut(zoomFactor);
	}
}
void MultiCamera::ZoomIn(float zoomFactor) {
	for (int i = 0; i < 4; i++) {
		cameras.at(i).ZoomIn(zoomFactor);
	}
}

void MultiCamera::setFieldOfView(float FoV) {
	fieldOfView = FoV;
	for (int i = 0; i < 4; i++) {
		cameras.at(i).setFieldOfView(FoV);
	}
}
void MultiCamera::setAspectRatio(float AR) {
	aspectRatio = AR;
	for (int i = 0; i < 4; i++) {
		cameras.at(i).setAspectRatio(AR);
	}
}
void MultiCamera::setNearVisionLimit(float NVL) {
	nearVisionLimit = NVL;
	for (int i = 0; i < 4; i++) {
		cameras.at(i).setNearVisionLimit(NVL);
	}
}

void MultiCamera::setFarVisionLimit(float FVL) {
	farVisionLimit = FVL;
	for (int i = 0; i < 4; i++) {
		cameras.at(i).setFarVisionLimit(FVL);
	}
}

int MultiCamera::getNCameras() {
	if (mode == viewMode::ONE_SCREEN) return 1;
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT) return 3;
	if (mode == viewMode::FOUR_CORNERS) return 4;
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT ) return 4;
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM) return 4;
	return 1;
}

void MultiCamera::setViewMode(viewMode _mode) {
	mode = _mode;
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT) {
		cameras.at(0).setAspectRatio((float)(width-(height/2))/(float)height);
		cameras.at(1).setAspectRatio(1.0f);
		cameras.at(2).setAspectRatio(1.0f);
	}
	else if(mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT 
			|| mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM){
		cameras.at(0).setAspectRatio((float)(width - (height / 2)) / (float)height);
		cameras.at(1).setAspectRatio(1.0f);
		cameras.at(2).setAspectRatio(2.0f);
		cameras.at(3).setAspectRatio(2.0f);
	}
	else {
		setAspectRatio((float)width / (float)height);
	}

	//zoom in for vertexing view if wanted
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM) cameras.at(3).ZoomIn(30.0f);

	UpdateBorderBuffers();
}

void MultiCamera::UpdateBorderBuffers() {
	std::vector< float >().swap(borderBuffer);
	std::vector< std::pair< int, int > >().swap(borderOffsetsAndLengths);

	if (mode == viewMode::ONE_SCREEN) return;
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT) {
		//right line
		borderBuffer.push_back( 1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back( 1.0f);
		borderBuffer.push_back( 1.0f);
		//bottom line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		//left line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		//top line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);

		borderOffsetsAndLengths.push_back(std::pair< int, int>(0,2));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(2,4));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(4,4));
	}
	if (mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT
			|| mode == viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM) {
		//right line
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		//bottom line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		//left line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		//bottom line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		//top line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		//left line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);

		borderOffsetsAndLengths.push_back(std::pair< int, int>(0, 2));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(2, 4));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(4, 6));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(8, 4));
	}
	if (mode == viewMode::FOUR_CORNERS) {
		//right line
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		//bottom line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		//left line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		//top line
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);
		//right line
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(-1.0f);
		borderBuffer.push_back(1.0f);
		borderBuffer.push_back(1.0f);

		borderOffsetsAndLengths.push_back(std::pair< int, int>(0, 4));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(2, 4));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(6, 4));
		borderOffsetsAndLengths.push_back(std::pair< int, int>(4, 4));
	}
	vertexBuffer->AddData(borderBuffer.data(), borderBuffer.size() * sizeof(float));
	vertexArray->AddBuffer(*vertexBuffer, layout);
}

void MultiCamera::DrawBorders(Renderer* r, Shader* s, unsigned int frameIndex) {
	if (mode == viewMode::ONE_SCREEN) return;

	GLCall(glLineWidth(0.2));
	s->Bind();
	s->SetUniform4f("u_Color", 0.3f, 0.3f, 0.3f, 1.0f);
	//2 floats per vertex here, so divide by 2
	r->DrawNoIB(*vertexArray, *s, GL_LINES, borderOffsetsAndLengths.at(frameIndex).second, borderOffsetsAndLengths.at(frameIndex).first);
}

MultiCamera::~MultiCamera() {
	delete vertexBuffer;
	delete vertexArray;
}