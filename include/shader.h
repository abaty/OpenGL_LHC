#pragma once
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include "glm/mat4x4.hpp"


struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map< std::string, int> m_UniformLocationCache;
	int GetUniformLocation(const std::string& name);

	ShaderProgramSource ParseShader(const std::string& filePath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, std::string fragmentShader);


public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Set Uniforms

	void SetUniform1ui(const std::string& name, unsigned int f);
	void SetUniform1f(const std::string& name, float f);
	void SetUniform3fv(const std::string& name, glm::vec3 values);
	void SetUniform3f(const std::string& name, float f1, float f2, float f3);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4x4f(const std::string& name, glm::mat4 values);
	void SetUniform3x3f(const std::string& name, glm::mat3 values);
	unsigned int GetRendererID() { return m_RendererID; }

};