#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>


//these headers are for mathemtical calculations (albeit for simple things you can use arrays of float and use a simple math library..)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader {

private:
	GLuint vShaderID;
	GLuint fShaderID;
	bool isProgLinked;
	std::string extractSrcCode(std::ifstream & shaderStream, std::stringstream & shaderSSstream);
	glm::vec3 lightDir;

public:
	GLuint shaderProgramID;
	Shader(const char * vShaderPath, const char * fShaderPath);
	//int loadVertexShader(const char * path);
	//int loadFragmentShader(const char* path);
	//int loadProgramShader(); //load program shader and link it with the current vertex, fragment shaders.
	bool isProgramLinked();
	void useProgram();
	void setMatrixUniform(const char * variableName, glm::mat4 matrix);
	void setVec3(const char* variableName, glm::vec3 vec);
	glm::vec3 getLightDir(){return lightDir;};


};