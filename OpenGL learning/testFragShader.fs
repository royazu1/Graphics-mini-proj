#version 330 core
out vec4 fragmentColor;
in vec3 vertexColor;

uniform int toggledFrag;

void main() {
	fragmentColor= vec4(vertexColor,1.0f);
	if (toggledFrag > 0) {
		fragmentColor=vec4(1.0f,0.0f, 1.0f, 1.0f); //magenta
	}
}
