#include "include/Renderer.h"
#include <iostream>

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << function << " " << file << " " << line << " " << std::endl;
		std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
		std::cin.get();
		return false;
	}
	return true;
}

Renderer::Renderer(bool doDepthBuffering, bool doAlphaBlending) 
{
	if (doDepthBuffering) GLCall(glEnable(GL_DEPTH_TEST));
	if (doAlphaBlending) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glEnable(GL_BLEND); 
		glClearColor(0.0, 0.0, 0.0, 0.0);
	}
}

void Renderer::DisableDepthBuffering() const {
	GLCall(glDisable(GL_DEPTH_TEST));
}

void Renderer::EnableDepthBuffering() const {
	GLCall(glEnable(GL_DEPTH_TEST));
}

void Renderer::DisableAlphaBlending() const {
	GLCall(glDisable(GL_BLEND));
}

void Renderer::EnableAlphaBlending() const {
	GLCall(glEnable(GL_BLEND));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(mode, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, unsigned int count) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	if (count > ib.GetCount()) {
		std::cout << "Renderer::Draw was given a custom count that is larger than the bound index buffer!  Setting it to the size of the index buffer!" << std::endl;
		count = ib.GetCount();
	}
	GLCall(glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, unsigned int count, unsigned int offset) const {
	shader.Bind();
	va.Bind();
	ib.Bind();
	if (count+offset > ib.GetCount()) {
		std::cout << "Renderer::Draw was given a custom count+offset that is larger than the bound index buffer!  Setting it to the size of the index buffer!" << std::endl;
		count = ib.GetCount();
	}
	GLCall(glDrawElements(mode, count, GL_UNSIGNED_INT, (void*) (offset * sizeof(unsigned int))));
}

void Renderer::Clear() const {
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}