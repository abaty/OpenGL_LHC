#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 inNormals;

out vec3 normal;
out vec3 fragPosition;
uniform mat4 u_ProjView;
uniform mat4 u_Rotation;
uniform mat4 u_Scale;
uniform mat4 u_Translate;
uniform mat3 u_NormalMatrix;

void main() {
	gl_Position = u_ProjView*u_Translate*u_Rotation*u_Scale*position;
	fragPosition = vec3(u_Translate*u_Rotation*u_Scale*position);
	normal = normalize(u_NormalMatrix*inNormals);
};

#shader fragment
#version 330 core
layout(location = 0) out vec4 color;

in vec3 normal;
in vec3 fragPosition;

struct Light {
	vec3 position;
	vec3 uniformLight;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	float shininess;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material u_material;
uniform Light u_light;

void main() {
	//ambient
	vec3 ambient = u_light.ambient * u_material.ambient;

	//diffuse
	vec3 lightDirection = normalize(u_light.position - fragPosition);
	float diff = max(dot(normal, lightDirection), 0.0);
	float diffUniform = max(dot(normal, u_light.position), 0.0);
	vec3 diffuseSpotLight = diff * u_light.diffuse;
	vec3 diffuseUniformLight = diffUniform * u_light.uniformLight;
	vec3 diffuse = (diffuseSpotLight + diffuseUniformLight) * u_material.diffuse;

	//specular
	vec3 viewDir = lightDirection; //(assume light is behind the viewer)
	//these lines for Blinn model
	//vec3 reflectDir = reflect(-lightDirection, normal);
	//vec3 reflectDir2 = reflect(-u_light.position, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_material.shininess);

	//these for Blinn-Phon model
	float spec = pow(max(dot(normal, viewDir), 0.0), u_material.shininess*3);
	float spec2 = pow(max(dot(normal, normalize(normalize(viewDir)+normalize(u_light.position))), 0.0), u_material.shininess*3);
	vec3 specLight = spec * u_light.specular + spec2*u_light.uniformLight;
	vec3 specular = specLight * u_material.specular;

	color = vec4(ambient+diffuse+specular, 1.0f);
};