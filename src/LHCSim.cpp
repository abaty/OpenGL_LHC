#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "include/Renderer.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include "include/VertexArray.h"
#include "include/shader.h"

#include "include/MultiCamera.h"
#include "include/MyEvent.h"
#include "include/Beam.h"


//NOTE that these lines switch from integrated graphics to NVIDIA graphics card
//undesirable because it breaks multi-platform compatibility (needs <windows.h>)
//not sure if there is a better solution at this point in time...
#include <windows.h>

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
//end of above note

int main(void)
{
	/* Initialize the library */
	if (!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	int aspectRatioX = 1280;
	int aspectRatioY = 720;
	//GLFWwindow* window = glfwCreateWindow(aspectRatioX, aspectRatioY, "LHCSim", NULL, NULL);

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	glfwWindowHint(GLFW_SAMPLES, 4);//multisampling for anti-aliasing

	aspectRatioX = mode->width;
	aspectRatioY = mode->height;
	GLFWwindow* window = glfwCreateWindow(aspectRatioX, aspectRatioY, "LHCSim", primaryMonitor, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error in GLEW initialization!" << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	std::cout << glGetString(GL_RENDERER) << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	std::cout << "Number of threads supported: " << std::thread::hardware_concurrency() << std::endl;

	MultiCamera multiCamera = MultiCamera(aspectRatioX, aspectRatioY, glm::vec3(0.0f, 3.0f, 10.0f));
	//multiCamera.setViewMode(viewMode::ONE_SCREEN);
	//multiCamera.setViewMode(viewMode::FOUR_CORNERS);
	//multiCamera.setViewMode(viewMode::ONE_LEFT_TWO_SQUARES_RIGHT);
	//multiCamera.setViewMode(viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT);
	multiCamera.setViewMode(viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM);

	Renderer renderer(true, true, true);

	Shader shader("resources/shaders/basic.shader");
	Shader trkShader("resources/shaders/trackShader.shader");
	Shader beamlineShader("resources/shaders/beamline.shader");
	Shader frameBorderShader("resources/shaders/frameBorder.shader");

	float secondToNSConversion = 2.0;//can think of this as an overall scale factor
	Beam beam = Beam("LHC", 0.05, secondToNSConversion);
	//beam.SetIsFixedTarget(1);
	//beam.SetNPipes(1);
	//beam.SetIsPoissonPU(false);
	beam.SetPileup(1);
	MCGenerator mcGen = MCGenerator(generatorType::PYTHIA8);
	MyEvent event = MyEvent(0.76, 0.96, &beam, &mcGen);
	//event.EnablePtCut(1.0f);
	//event.EnableEtaCut(2.4f);

	bool doZoomIn = false;
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	unsigned int viewportSwaps = 0;

	beam.Start();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		//check frame rate
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime =currentTime;
		}

		//stuff that is done before actually rendering
		//camera control
		multiCamera.cameras.at(0).Rotate(0.0075f);
		if (doZoomIn) multiCamera.cameras.at(0).ZoomIn(1.0015f);
		else          multiCamera.cameras.at(0).ZoomOut(1.0015f);
		if (multiCamera.cameras.at(0).getDistanceFromCenterOfWorld() > 15.1f) doZoomIn = true;
		if (multiCamera.cameras.at(0).getDistanceFromCenterOfWorld() < 1.0f) doZoomIn = false;

		//beamline and event updates
		beam.UpdateTiming();
		/*if (beam.GetBPTXFlag()) {
			multiCamera.setViewMode((viewMode)(viewportSwaps % 4));
			viewportSwaps++;
		}*/
		event.Update();

		/* Render here */
		renderer.Clear();

		for (int i = 0; i < multiCamera.getNCameras(); i++) {
			multiCamera.setViewport(i);

			shader.Bind();
			shader.SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
			glm::mat4 projView = multiCamera.cameras.at(i).getProjectionViewMatrix();
			shader.SetUniform4x4f("u_Rotation", projView);

			//beamline drawing
			beamlineShader.Bind();
			beamlineShader.SetUniform4x4f("u_Rotation", projView);
			if (multiCamera.cameras.at(i).GetDoShowBeamPipe()) {
				beam.Draw(&renderer, &beamlineShader);
			}

			trkShader.Bind();
			trkShader.SetUniform4x4f("u_Rotation", projView);

			//draw event
			if(!event.GetIsSettingUp()) event.Draw( &renderer, &trkShader);

			//draw any frame borders if needed
			multiCamera.DrawBorders(&renderer, &frameBorderShader, i);
		}

		GLCall(glfwSwapBuffers(window)); 		/* Swap front and back buffers */
		GLCall(glfwPollEvents());			/* Poll for and process events */
	}
	glfwTerminate();

	std::cout << "hi" << std::endl;
	std::cin.get();
}