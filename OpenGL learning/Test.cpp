
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <ostream>
#include <iostream>

#include "stb_image.h"
#include "Shader.h"
#include "Map.h"

//these headers are for mathemtical calculations (albeit for simple things you can use arrays of float and use a simple math library..)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keypress_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void rotate_cam_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


Scene scene;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //VERSION 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if (window == NULL) {
		std::cout << "COULD NOT CREATE A WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); //Make the window context we created CURRENT

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, rotate_cam_key_callback);
	//----------------------------------------------------------------------------------------------------------------INIT 
	//set keyboard and mouse callbacks for changing camera position and orientation
//	glfwSetCursorPosCallback(window, mouse_callback); //cursor position callback for changing view matrix according to cursor movements
	//glfwSetScrollCallback(window, scroll_callback);
	//glfwSetKeyCallback(window, keypress_callback);

	Shader myShader = Shader("testVertexShader.vs", "testFragShader.fs");
	

	
	Map myMap = Map("worldmap.png");

	myMap.create3DMeshData();
	myMap.createMesh(0,scene); //RES is unusued for now
	

	/*
	
	struct vertex {
		float position[3];
		float color[3];
	};

	struct vertex v1;
	v1.position[0] = 0.5f;
	v1.position[1] = 0.5f;
	v1.position[2] = 0.0f;

	v1.color[0] = 1.0f;
	v1.color[1] = 0.0f;
	v1.color[2] = 0.0f;


	struct vertex v2;
	v2.position[0] = -0.5f;
	v2.position[1] = 0.5f;
	v2.position[2] = 0.0f;
	v2.color[0] = 1.0f;
	v2.color[1] = 0.0f;
	v2.color[2] = 0.0f;

	struct vertex v3;
	v3.position[0] = 0.2f;
	v3.position[1] = 0.0f;
	v3.position[2] = 0.0f;
	v3.color[0] = 1.0f;
	v3.color[1] = 0.0f;
	v3.color[2] = 0.0f;

	struct vertex v4;
	v4.position[0] = -0.2f;
	v4.position[1] = 0.0f;
	v4.position[2] = 0.0f;
	v4.color[0] = 1.0f;
	v4.color[1] = 0.0f;
	v4.color[2] = 0.0f;


	struct vertex vArr[4];
	vArr[0] = v1;
	vArr[1] = v2;
	vArr[2] = v3;
	vArr[3] = v4;

	float vertices[] = {
		// positions          // colors           // texture coords
	   0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
	   0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
	  -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f

	};

	int renderIndexes[] = { 0,2,1,
						0,3,1 };



	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(renderIndexes), renderIndexes, GL_STATIC_DRAW);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vArr), vArr, GL_STATIC_DRAW);

	printf("%d", sizeof(vArr));
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(3 * sizeof(float))); //remember the last arg is offset in bytes!
	glEnableVertexAttribArray(1);

	
	*/

	//glUseProgram(shaderProgram); // causes the shader executables to become part of the current openGL context

	//render loop
	while (!glfwWindowShouldClose(window)) { //as long as this windows closing flag is not set
		//if linking succeeded, we attach the shader executable to the rendering context:
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT );          // DONT FORGET TO RE-ENABLE DEPTH TESTING
		myShader.useProgram();
		//glBindVertexArray(vao);


		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 camSpaceTrans = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glm::frustum
		myShader.setMatrixUniform("camTransMatrix", camSpaceTrans);
		myShader.setMatrixUniform("projectionMatrix", projection);
		glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT); //cam view rendering

		scene.Draw(1);
		printf("drawn\n");


		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		camSpaceTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		myShader.setMatrixUniform("camTransMatrix", camSpaceTrans);
		myShader.setMatrixUniform("projectionMatrix", projection);

		glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT); //global view rendering
		scene.Draw(0);

		glfwSwapBuffers(window); //swap buffers for the current contexted window
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;

}



void rotate_cam_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_RIGHT: //perform yaw - clockwise say 6 degrees
			yaw += 6.0f;
		//	if (yaw >= 89.0f) { yaw = 89.0f; }
			break;
		case GLFW_KEY_LEFT: //perform yaw - counter-clockwise
			yaw -= 6.0f;
			//if (yaw <= -89.0f) { yaw = -89.0f; }
			break;
		case GLFW_KEY_UP:
			pitch += 6.0f;
			if (pitch > 89.0f) { pitch = 89.0f; }
			break;
		case GLFW_KEY_DOWN:
			pitch -= 6.0f;
			if (pitch < -89.0f) { pitch = -89.0f; }
			break;
		case GLFW_KEY_W:
			cameraPos = cameraPos + cameraFront; //speed
			break;
		case GLFW_KEY_S:
			cameraPos = cameraPos - cameraFront;
			break;
		case GLFW_KEY_R: //record camera position and display it as a rendered point at the GLOBAL VIEW (LEFT VIEWPORT)
			scene.addCamPosRenderVAO(cameraPos, cameraFront);
			break;
		}
		

	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordinglyW
// ---------------------------------------------------------------------------------------------------------
void keypress_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	float cameraSpeed = static_cast<float>(2.5 * 0.01);

	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
		case GLFW_KEY_W:
			cameraPos += cameraSpeed * cameraFront;
			break;
		case GLFW_KEY_S:
			cameraPos -= cameraSpeed * cameraFront;
			break;
		case GLFW_KEY_D:
			cameraPos += cameraSpeed * glm::cross(cameraFront, cameraUp); //note order of cross prod vecs
			break;
		case GLFW_KEY_A:
			cameraPos -= cameraSpeed * glm::cross(cameraFront, cameraUp); //note order of cross prod vecs
			break;
		}
	
	}

	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since positive offset is with clock direction i.e negative pitch
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}