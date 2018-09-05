#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in float indexAlongTrack;
layout(location = 2) in float PID;

uniform mat4 u_Rotation;

uniform float u_fractionOfPropagationTimeElapsed; //what fraction of the allotted propagation time has elapsed
uniform float u_minAlpha; //min alpha a track is faded out to
uniform float u_pointsOnTrack; //should be the same as the value in the Event class for the number of points on a track
uniform float u_nPointsOfGradient; //number of points to fade the track from the max alpha to the min alpha over
uniform float u_alphaModifierForFade;//determines how much to fade at the end of the animation
uniform float u_inverseLengthScale;//determines the scaling from the vtxArray coordinates to 'world' coordinates

uniform vec4 u_ColorDefault;
uniform vec4 u_ColorPion;
uniform vec4 u_ColorKaon;
uniform vec4 u_ColorProton;
uniform vec4 u_ColorElectron;
uniform vec4 u_ColorMuon;

out vec4 inputColor;

void main() {
	vec4 tempPosition = position;
	tempPosition.xyz = u_inverseLengthScale*tempPosition.xyz;
	gl_Position = u_Rotation * (tempPosition);

	//don't draw stuff on the 'outside' of the track first
	float alpha = 0.0f; 
	float difference = u_fractionOfPropagationTimeElapsed * u_pointsOnTrack - indexAlongTrack;
	if( difference > 0 ){
		alpha = u_minAlpha + (1 - u_minAlpha) * (1 - difference / u_nPointsOfGradient);
		if (difference > u_nPointsOfGradient) {
			alpha = u_minAlpha;
		}
	}
	alpha = alpha * u_alphaModifierForFade;

	if      (PID - 11   < 0.01f)   inputColor = u_ColorElectron;
	else if (PID - 13   < 0.01f)   inputColor = u_ColorMuon;
	else if (PID - 211  < 0.01f)   inputColor = u_ColorPion;
	else if (PID - 321  < 0.01f)   inputColor = u_ColorKaon;
	else if (PID - 2212 < 0.01f)   inputColor = u_ColorProton;
	else				        inputColor = u_ColorDefault;
	inputColor[3] = alpha;
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec4 inputColor;

void main() {
	color = inputColor;
};
