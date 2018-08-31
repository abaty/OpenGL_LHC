#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in float alpha;

uniform mat4 u_Rotation;
uniform float u_fadeStartDist;
uniform float u_fadeEndDist; // do not put in points past this value or strange things might happen
out float alphaModifier;

void main() {
	gl_Position = u_Rotation*position;
	alphaModifier = 1.0f;
	float difference = abs(position[0]) - u_fadeStartDist;
	if (difference > 0) alphaModifier = 1.0f - difference / (u_fadeEndDist - u_fadeStartDist);
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_Color;
in float alphaModifier;

void main() {
	color = u_Color;
	color[3] = alphaModifier * u_Color[3];
};