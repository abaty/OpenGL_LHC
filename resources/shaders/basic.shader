#shader vertex
#version 330 core
layout(location = 0) in vec4 position;

out float z;
uniform mat4 u_Rotation;

void main(){
    gl_Position =  u_Rotation*position;
	z = position.z;
}

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_Color;
in float z;

void main(){
    color = u_Color;
	color.r = 4*z*z;
}
