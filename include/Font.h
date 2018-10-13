#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "include/Renderer.h"
#include "include/shader.h"
#include <string>
#include <map>
#include <iostream>

struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class Font {
public:
	Font(int screenWidth, int screenHeight, int _fontHeight ,std::string font = "resources/Fonts/arial.ttf");
	void UpdateScreenSize(int screenWidth, int screenHeight);
	void RenderText(Shader *s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

	inline int getFontHeight() { return fontHeight; }
private:
	std::map<GLchar, Character> characters;
	glm::mat4 fontProjection;

	GLuint VAO;
	GLuint VBO;

	int fontHeight;
};