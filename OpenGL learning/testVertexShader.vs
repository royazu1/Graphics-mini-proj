#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
//layout (location = 2) in vec3 normal;
uniform mat4 projectionMatrix;
uniform mat4 camTransMatrix;
out vec3 vertexColor;

//uniform vec3 lightDir;


void main() {
	gl_Position= projectionMatrix * camTransMatrix * vec4(position, 1.0f);
	vertexColor=color;
}