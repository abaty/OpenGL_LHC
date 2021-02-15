#shader vertex
#version 330 core
layout(location = 0) in vec4 position;

uniform mat4 u_Rotation;

uniform float u_fadeStartDist;
uniform float u_fadeEndDist; // do not put in points past this value or strange things might happen

uniform vec4 u_PipeColor;
uniform vec4 u_BunchColor;

uniform float u_BunchSpacing;
uniform float u_2BunchLengthSquared;
uniform float u_BunchCenter;

uniform uint u_bothWays;

out vec4 outColor;

void main() {
	gl_Position = u_Rotation*position;

	//calculate the relevant bunch center for the gaussian
	float bunchCenter = u_BunchCenter + floor((position[0] - u_BunchCenter) / u_BunchSpacing + 0.5) * u_BunchSpacing;

	//get a gaussian value (change to triangular if slow?)
	float gaussianValue = exp(-(position[0] - bunchCenter)*(position[0] - bunchCenter) / u_2BunchLengthSquared);
	//gl_Position.y = gaussianValue;

	//if we want the beamline going both ways
	if (u_bothWays == uint(1)) {
		float bunchCenter2 = u_BunchCenter + floor((-position[0] - u_BunchCenter) / u_BunchSpacing + 0.5) * u_BunchSpacing;
		float gaussianValue2 = exp(-(-position[0] - bunchCenter2)*(-position[0] - bunchCenter2) / u_2BunchLengthSquared);
		gaussianValue = max(gaussianValue, gaussianValue2);
	}

	//calculate fade for end of pipes
	float alphaModifier = 1.0f;
	float difference = abs(position[0]) - u_fadeStartDist;
	if (difference > 0) alphaModifier = 1.0f - difference / (u_fadeEndDist - u_fadeStartDist);

	outColor.xyz = gaussianValue * u_BunchColor.xyz + (1-gaussianValue) * u_PipeColor.xyz + 0.2*gaussianValue;
	outColor[3] = alphaModifier;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec4 outColor;

void main() {
	color = outColor;
}
