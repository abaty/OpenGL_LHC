#pragma once
#include <GL/glew.h>
#include "include/VertexArray.h"
#include "include/IndexBuffer.h"
#include "include/shader.h"
#include "include/camera.h"

//Visual studio specific stuff, might be bad for working with other compilers
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer {

public:
	Renderer(bool doDepthBuffering = true, bool doAlphaBlending = true, bool doAntiAliasing = true);
	
	void DisableDepthBuffering() const;
	void EnableDepthBuffering() const;

	void DisableAlphaBlending() const;
	void EnableAlphaBlending() const;

	void EnableAntiAliasing() const;
	void DisableAntiAliasing() const;

	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode) const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, 
		unsigned int count) const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, 
		unsigned int count, unsigned int offset) const;
	void MultiDraw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, GLenum mode, 
		GLsizei* counts, GLvoid* offsets, unsigned int nPrimitives) const;

	//drawing without an index buffer (for stuff that is in order 0-nVtxs)
	void DrawNoIB(const VertexArray& va, const Shader& shader, GLenum mode, unsigned int count) const;
	void DrawNoIB(const VertexArray& va, const Shader& shader, GLenum mode, unsigned int count, unsigned int offset) const;
	void MultiDrawNoIB(const VertexArray& va, const Shader& shader, GLenum mode, GLint* count, GLsizei* offset, GLsizei _nPrimitives) const;

};

//end done