#include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "include/GlobalSettings.h"
#include "include/Renderer.h"
#include "include/VertexBuffer.h"
#include "include/VertexBufferLayout.h"
#include "include/IndexBuffer.h"
#include "include/VertexArray.h"
#include "include/shader.h"
#include "include/Font.h"
#include "include/Geometry/Box3D.h"
#include "include/Geometry/Prism3D.h"
#include "include/Geometry/Extrusion3D.h"
#include "include/Geometry/Tube3D.h"
#include "include/Geometry/PolygonBuilder.h"

#include "include/MultiCamera.h"
#include "include/MyEvent.h"
#include "include/Beam.h"
#include "include/Solenoid.h"
#include "include/BFieldMap.h"

//NOTE that these lines switch from integrated graphics to NVIDIA graphics card
//undesirable because it breaks multi-platform compatibility (needs <windows.h>)
//not sure if there is a better solution at this point in time...
#include <windows.h>

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
//end of above note

GlobalSettings settings = GlobalSettings();

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

	MultiCamera multiCamera = MultiCamera(aspectRatioX, aspectRatioY, glm::vec3(3.0f, 1.0f, 1.0f));
	//multiCamera.setViewMode(viewMode::ONE_SCREEN);
	//multiCamera.setViewMode(viewMode::FOUR_CORNERS);
	//multiCamera.setViewMode(viewMode::ONE_LEFT_TWO_SQUARES_RIGHT);
	//multiCamera.setViewMode(viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT);
	multiCamera.setViewMode(viewMode::ONE_LEFT_TWO_SQUARES_RIGHT_BOTTOMLEFTSPLIT_ZOOM);

	Font arial = Font(aspectRatioX, aspectRatioY, aspectRatioY/60.0);
	Renderer renderer(true, true);

	Shader boxShader("resources/shaders/geometryBox.shader");
	Shader trkShader("resources/shaders/trackShader.shader");
	Shader beamlineShader("resources/shaders/beamline.shader");
	Shader frameBorderShader("resources/shaders/frameBorder.shader");
	Shader fontShader("resources/shaders/fontShader.shader");

	float secondToNSConversion = 1.0f;//can think of this as an overall scale factor
	Beam beam = Beam("LEP", 0.1f, secondToNSConversion);
	//beam.SetIsFixedTarget(1);
	beam.SetNPipes(2);
	//beam.SetIsPoissonPU(false);
	beam.SetPileup(2);
	MCGenerator mcGen = MCGenerator(generatorType::PYTHIA8);
	//MCGenerator mcGen = MCGenerator(generatorType::ISOTROPIC);
	BFieldMap bMap = BFieldMap();
	Solenoid sol = Solenoid(10, 22e6, 1, 2.0, 2.5, glm::vec3(1.0, 0.0, 0), glm::vec3(0,0,0));
	Solenoid sol2 = Solenoid(10, 42e6, 1, 0.8, 1.0, glm::vec3(-1.0f, 0.0f,0), glm::vec3(0,0,0));
	bMap.addMagnet(&sol);
	bMap.addMagnet(&sol2);

	MyEvent event = MyEvent(0.76, 0.96, &beam, &mcGen, &bMap);
	//event.EnablePtCut(1.0f);
	event.EnableEtaCut(1.5f);

	bool doZoomIn = false;
	unsigned int viewportSwaps = 0;

	beam.Start();

	Box3D b = Box3D(0.0f,0.0f,1.0f,0.1f,3.0f,1.0f);
	b.setMaterial(matEnum::BLACK_PLASTIC);
	b.setRotZ(-0.4);
	Box3D b2 = Box3D(1.2f,1.2f,1.2f,0.1f,0.1f,0.1f);
	Box3D b3 = Box3D(1.0f, 1.0f, 1.0f, 2.5f, 0.1f, 0.1f);
	Box3D b4 = Box3D(0.0f, 0.0f, 0.0f, 0.1f, 0.1f, 0.1f);
	b4.setMaterial(matEnum::JADE);

	PolygonBuilder pb = PolygonBuilder();
	//Prism3D prism = Prism3D(pb.nGon(4,0.5), pb.nGon(4,-0.5), 2, -1.0, -1.0, -1.0, 0.3f, 0.6f, 2.0f);
	//Prism3D prism = Prism3D(pb.nGon(100,0.5), pb.nGon(100, 0.5), 2, -1.0, -1.0, -1.0, 0.3f, 0.6f, 2.0f);
	//Extrusion3D prism = Extrusion3D(pb.nGon(30,0.5,0.2), pb.nGon(30,-0.5,1.2), 5, 2, -1.0, -1.0, -1.0, 1.0f, 1.0f, 1.0f);
	Tube3D prism = Tube3D(pb.nGon(30, 0.5, 0.5), pb.nGon(30, -0.5, 0.8), 1, 2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
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

		b.setOffX((float)multiCamera.cameras.at(0).getDistanceFromCenterOfWorld()/4.0);
		b4.setOffY((float)multiCamera.cameras.at(0).getDistanceFromCenterOfWorld() / 15.0);
		b4.setOffZ((float)multiCamera.cameras.at(0).getDistanceFromCenterOfWorld() / 15.0);

		for (int i = 0; i < multiCamera.getNCameras(); i++) {
			multiCamera.setViewport(false, i);
			
			b.setUniforms(&boxShader);
			glm::mat4 projView = multiCamera.cameras.at(i).getProjectionViewMatrix();
			boxShader.SetUniform4x4f("u_ProjView", projView);
			boxShader.SetUniform3fv("u_light.position", multiCamera.cameras.at(i).getPosition());

			if (b.isInside(1.2,1.2,1.2) == 1) b.setMaterial(matEnum::RED_PLASTIC);
			//else b.setMaterial(matEnum::BLACK_PLASTIC);
			else if (b.isInside(&b3) || b3.isInside(&b)) b.setMaterial(matEnum::RED_PLASTIC);
			else b.setMaterial(matEnum::BLACK_PLASTIC);
			
			if (prism.isInside(&b4)) b4.setMaterial(matEnum::CYAN_PLASTIC);
			else b4.setMaterial(matEnum::BLACK_PLASTIC);
			if (prism.isInside(0, (float)multiCamera.cameras.at(0).getDistanceFromCenterOfWorld() / 15.0 , (float)multiCamera.cameras.at(0).getDistanceFromCenterOfWorld() / 15.0)) prism.setMaterial(matEnum::RED_PLASTIC);
			else prism.setMaterial(matEnum::OBSIDIAN);
			b.Draw(&renderer, &boxShader);
			b2.setUniforms(&boxShader);
			b2.Draw(&renderer, &boxShader);
			b3.setUniforms(&boxShader);
			b3.Draw(&renderer, &boxShader);
			b4.setUniforms(&boxShader);
			b4.Draw(&renderer, &boxShader);
			prism.setUniforms(&boxShader);
			prism.Draw(&renderer, &boxShader);

			//beamline drawing
			beamlineShader.Bind();
			beamlineShader.SetUniform4x4f("u_Rotation", projView);
			if (multiCamera.cameras.at(i).GetDoShowBeamPipe()) {
				beam.Draw(&renderer, &beamlineShader);
			}

			trkShader.Bind();
			trkShader.SetUniform4x4f("u_Rotation", projView);

			//draw event
			if(!event.GetIsSettingUp() && beam.GetIsStarted()) event.Draw( &renderer, &trkShader);

			//draw any frame borders if needed
			multiCamera.DrawBorders(&renderer, &frameBorderShader, i);
		}

		if (settings.doFPS) {
			multiCamera.setViewport(true, 0);
			glm::vec3 fontColor = glm::vec3(0.5f, 0.5f, 0.5f);
			std::string fps = "FPS: " + std::to_string(settings.getFPS());
			int decimalPlace = fps.find(".");
			arial.RenderText(&fontShader, fps.substr(0,decimalPlace) , aspectRatioX*0.955f, aspectRatioY-arial.getFontHeight()*1.1f, 1.0f ,fontColor);
		}

		GLCall(glfwSwapBuffers(window)); 		/* Swap front and back buffers */
		GLCall(glfwPollEvents());			/* Poll for and process events */
	}
	glfwTerminate();

	std::cout << "hi" << std::endl;
	std::cin.get();
}