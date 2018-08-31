#pragma once
#include <GL/glew.h>
#include "include/VertexArray.h"
#include "include/IndexBuffer.h"
#include "include/shader.h"

//Visual studio specific stuff, might be bad for working with other compilers
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {

public:
	Renderer(bool doDepthBuffering = true, bool doAlphaBlending = true);
	
	void DisableDepthBuffering() const;
	void EnableDepthBuffering() const;

	void DisableAlphaBlending() const;
	void EnableAlphaBlending() const;

	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode) const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, unsigned int count) const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, unsigned int count, unsigned int offset) const;
};

//end done