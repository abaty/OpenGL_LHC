#pragma once

class VertexBuffer {

private:
	unsigned int m_RendererID;

public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void AddData(const void* data, unsigned int size);
	void AddDataDynamic(const void* data, unsigned int size);
	void SubData(const void* data, unsigned int size, unsigned int offset);

	void Bind() const;
	void Unbind() const;

};