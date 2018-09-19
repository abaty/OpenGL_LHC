#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "include/Renderer.h"
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
	Font(int screenWidth, int screenHeight, std::string font = "resources/Fonts/arial.ttf");
	void UpdateScreenSize(int screenWidth, int screenHeight);

private:
	std::map<GLchar, Character> characters;
	glm::mat4 fontProjection;
};