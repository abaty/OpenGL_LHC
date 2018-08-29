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

#include "include/Camera.h"
#include "include/Event.h"


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
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	int aspectRatioX = 1280;
	int aspectRatioY = 720;
	GLFWwindow* window = glfwCreateWindow(aspectRatioX, aspectRatioY, "LHCSim", NULL, NULL);
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

	float positions[12] = { -0.5f, -0.5f, 0.2f, 0.5f, -0.5f, 0.1f, 0.5f, 0.5f, 0.1f, -0.5f, 0.5f, -0.8f};
	unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

	float beamline[6] = { -999.0f, 0, 0, 999.0f, 0 ,0};
	unsigned int beamIndices[2] = { 0, 1};

	Camera camera = Camera( glm::vec3(0.0f, 1.0f, 4.0f) );
	camera.setAspectRatio((float)aspectRatioX/(float)aspectRatioY);

	VertexArray va;
	VertexBuffer *vb = new VertexBuffer(positions, 4 * 3 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(3);
	va.AddBuffer(*vb, layout);
	IndexBuffer *ib = new IndexBuffer(indices, 6);

	VertexArray beamLine_va;
	VertexBuffer *beamLine_vb = new VertexBuffer(beamline, 2*3*sizeof(float));
	beamLine_va.AddBuffer(*beamLine_vb, layout);
	IndexBuffer *beamLine_ib = new IndexBuffer(beamIndices, 2);

	Shader shader("resources/shaders/basic.shader");
	Shader trkShader("resources/shaders/trackShader.shader");

	Renderer renderer;

	Event event = Event(glfwGetTime(),10,14,15);

	float r = 0.0f;
	float  increment = 0.05f;

	double lastTime = glfwGetTime();
	int nbFrames = 0;

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

		/* Render here */
		renderer.Clear();

		shader.Bind();
		shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

		camera.Rotate(0.0075f);
		glm::mat4 projView = camera.getProjectionViewMatrix();
		shader.SetUniform4x4f("u_Rotation", projView);
		//renderer.Draw(va, *ib, shader, GL_TRIANGLES);

		shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);
		GLCall(glLineWidth(1));
		renderer.Draw(beamLine_va, *beamLine_ib, shader, GL_LINES);

		trkShader.Bind();
		GLCall(glLineWidth(2));
		trkShader.SetUniform4x4f("u_Rotation", projView);
		trkShader.SetUniform4f("u_Color", 0.1f, 0.8f, 0.1f, 0.0f);
		trkShader.SetUniform1f("u_fractionOfPropagationTimeElapsed", (float) (currentTime-event.GetStartTime())/(event.GetAnimationTime()));
		trkShader.SetUniform1f("u_minAlpha", 0.5f);
		trkShader.SetUniform1f("u_pointsOnTrack", (float) event.GetNTimeIntervals());
		trkShader.SetUniform1f("u_nPointsOfGradient", 0.025f * event.GetNTimeIntervals());
		trkShader.SetUniform1f("u_alphaModifierForFade", event.getAlphaModiferForFade());

		//update event and draw it
		if( event.Update(glfwGetTime()) ) event.SetupDraw();
		event.Draw( &renderer, &trkShader);

		if (r > 1.0f) increment = -0.05f;
		if (r < 0.0f) increment = 0.05f;
		r += increment;

		GLCall(glfwSwapBuffers(window)); 		/* Swap front and back buffers */
		GLCall(glfwPollEvents());			/* Poll for and process events */
	}

	delete vb;
	delete ib;
	delete beamLine_vb;
	delete beamLine_ib;

	glfwTerminate();

	std::cout << "hi" << std::endl;
	std::cin.get();
}