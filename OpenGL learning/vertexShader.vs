#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
void main() { 
ourColor= vec3(aColor);
gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0f);
TexCoord= vec2(aTexCoord.x , aTexCoord.y );
} 