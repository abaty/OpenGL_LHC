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
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	int aspectRatioX = 1280;
	int aspectRatioY = 720;

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	//GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "LHCSim", primaryMonitor, NULL);
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

	float positions[12] = { -1.1f, 0.1f, 1.1f, 1.1f, 0.0f, 1.1f, 1.1f, 0.0f, -1.1f, -1.1f, 0.0f, -1.1f};
	unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

	Camera camera = Camera( glm::vec3(0.0f, 5.0f, 10.0f) );
	camera.setAspectRatio((float)mode->width/(float)mode->height);

	VertexArray va;
	VertexBuffer *vb = new VertexBuffer(positions, 4 * 3 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(3);
	va.AddBuffer(*vb, layout);
	IndexBuffer *ib = new IndexBuffer(indices, 6);

	Shader shader("resources/shaders/basic.shader");
	Shader trkShader("resources/shaders/trackShader.shader");
	Shader beamlineShader("resources/shaders/beamline.shader");

	Renderer renderer;

	float secondToNSConversion = 1.0;//can think of this as an overall scale factor
	Beam beam = Beam("LHC", secondToNSConversion);
	Event event = Event(glfwGetTime(), 0.76, 0.96, &beam);

	float r = 0.0f;
	float  increment = 0.05f;

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	beam.Start(glfwGetTime());

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

		//beamline drawing
		beamlineShader.Bind();
		beamlineShader.SetUniform4x4f("u_Rotation", projView);
		//beam.Update(glfwGetTime());
		beam.Draw(&renderer, &beamlineShader);

		trkShader.Bind();
		trkShader.SetUniform4x4f("u_Rotation", projView);

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

	glfwTerminate();

	std::cout << "hi" << std::endl;
	std::cin.get();
}