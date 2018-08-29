#include "include/VertexBuffer.h"
#include "include/Renderer.h"
#include <iostream>

VertexBuffer::VertexBuffer() {
	GLCall(glGenBuffers(1, &m_RendererID));
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size){
	GLCall(glGenBuffers(1, &m_RendererID));
	AddData(data, size);
}

VertexBuffer::~VertexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::AddData(const void* data, unsigned int size) {
	Bind();
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}