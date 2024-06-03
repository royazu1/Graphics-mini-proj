
#include "Shader.h"


Shader::Shader(const char* vShaderPath, const char* fShaderPath) {
	vShaderID = -1;
	fShaderID = -1;
	shaderProgramID = -1;
	isProgLinked = false;

	GLuint vertexShader=-1;
	GLuint fragmentShader=-1;
	std::ifstream vShaderStream;
	std::ifstream fShaderStream;


	std::stringstream ssStreamVShader;
	std::stringstream ssStreamFShader;
	//std::stringstream fShaderStrStream;

	vShaderStream.open(vShaderPath);
	fShaderStream.open(fShaderPath);

	char info_log[512];
	int compileStatus;

	if (vShaderStream.is_open()) {
		ssStreamVShader << vShaderStream.rdbuf();

		std::string srcString = ssStreamVShader.str();
		const char* vShaderSRC = srcString.c_str();
		vertexShader= glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vShaderSRC, NULL);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
		
		if (!compileStatus) {
			glGetShaderInfoLog(vertexShader, 512, NULL, info_log);
			std::cout << "V shader error.." << std::endl;
			std::cout << info_log << std::endl;
		}
		
	}
	else {
		std::cout << "Couldn't open vertex shader src file.." << std::endl;
	}

	if (fShaderStream.is_open()) {
		ssStreamFShader << fShaderStream.rdbuf();
		std::string srcString = ssStreamFShader.str();
		const char* fShaderSRC = srcString.c_str();
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fShaderSRC, NULL);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);

		if (!compileStatus) {
			glGetShaderInfoLog(fragmentShader, 512, NULL, info_log);
			std::cout << "F shader error.." << std::endl;
			std::cout << info_log << std::endl;
		}

	}
	else {
		std::cout << "Couldn't open fragment shader src file.." << std::endl;
	}


	//now create program object and link it with the linked shaders
	int linkStatus;
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &linkStatus);

	if (!linkStatus) {
		glad_glGetProgramInfoLog(shaderProgram, 512, NULL, info_log);
		std::cout << info_log << std::endl;
	}
	else {
		isProgLinked = true;
		shaderProgramID = shaderProgram;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

std::string Shader::extractSrcCode(std::ifstream& shaderStream, std::stringstream& shaderSSstream) {
	shaderSSstream << shaderStream.rdbuf();
	std::string srcCodeString = shaderSSstream.str();
	return srcCodeString;
}

void Shader::setIntegerUniform(const char* variableName, int val) {
	glUniform1i(glGetUniformLocation(shaderProgramID, variableName), val);
}



void  Shader::setMatrixUniform(const char * variableName,glm::mat4 matrix) {
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, variableName), 1, GL_FALSE, glm::value_ptr(matrix)); //query location the send that data, program object should be bound by then.
}

void Shader::setVec3(const char* variableName,glm::vec3 vec)
{
	glUniform3f(glGetUniformLocation(shaderProgramID, variableName), vec.x,vec.y, vec.z);
}


bool Shader::isProgramLinked() {
	return isProgLinked;
}

void Shader::useProgram() {
	glUseProgram(shaderProgramID);
}

