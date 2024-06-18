#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;
layout(location = 3) in vec3 aNormal;
uniform mat4 projectionMatrix;//projection
uniform mat4 camTransMatrix;//view
//light
uniform mat4 model;
//
out vec3 vertexColor;
out vec3 Normal;

uniform vec3 lightDir;


void main() {
	FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;

	gl_Position= projectionMatrix * camTransMatrix * vec4(position, 1.0f);
	vertexColor=color;	
}