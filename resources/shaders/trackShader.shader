#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in float indexAlongTrack;

uniform mat4 u_Rotation;
uniform float u_fractionOfPropagationTimeElapsed; //what fraction of the allotted propagation time has elapsed
uniform float u_minAlpha; //min alpha a track is faded out to
uniform float u_pointsOnTrack; //should be the same as the value in the Event class for the number of points on a track
uniform float u_nPointsOfGradient; //number of points to fade the track from the max alpha to the min alpha over
uniform float u_alphaModifierForFade;

out float alpha;

void main() {
	gl_Position = u_Rotation*position;

	//don't draw stuff on the 'outside' of the track first
	alpha = 0.0f; 
	float difference = u_fractionOfPropagationTimeElapsed * u_pointsOnTrack - indexAlongTrack;
	if( difference > 0 ){
		alpha = u_minAlpha + (1 - u_minAlpha) * (1 - difference / u_nPointsOfGradient);
		if (difference > u_nPointsOfGradient) {
			alpha = u_minAlpha;
		}
	}
	alpha = alpha * u_alphaModifierForFade;
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;
in float alpha;

uniform vec4 u_Color;

void main() {
	color = u_Color;
	color[3] = alpha;
};
