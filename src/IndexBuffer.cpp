#include "include/IndexBuffer.h"
#include "include/Renderer.h"

IndexBuffer::IndexBuffer(){
	GLCall(glGenBuffers(1, &m_RendererID));
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count){
	GLCall(glGenBuffers(1, &m_RendererID));
	AddData(data, count);
}

IndexBuffer::~IndexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::AddData(const unsigned int* data, unsigned int count) {
	m_Count = count;
	Bind();
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

void IndexBuffer::SubData(const unsigned int* data, unsigned int count, unsigned int offset) {
	m_Count = count;
	Bind();
	GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * sizeof(unsigned int), data));
}

void IndexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}