
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
void mouse_picking_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_cursor_picking_callback(GLFWwindow* window, double xposIn, double yposIn);
// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 oldCameraPos; //this is to backup the camera-pos before we entered toggle mode
glm::vec3 oldCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);


bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
std::vector<glm::uvec2> pickedPosVec;

bool isToggled = false;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int pickingX=-1;
int pickingY=-1;

Scene scene;

bool pixelPicked = false;
glm::uvec2 pickedPos;
int picked = 0;


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
	//glfwSetMouseButtonCallback(window, mouse_picking_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); //REMOVE AFTER DEBUGGING

	glfwSetCursorPosCallback(window, mouse_cursor_picking_callback);
	Shader myShader = Shader("testVertexShader.vs", "testFragShader.fs");
	

	
	Map myMap = Map("heatmap.jpg");

	myMap.create3DMeshData();
	myMap.createMesh(0,scene); //RES is unusued for now
	
	glm::vec3 pickedColorArr[3];
	pickedColorArr[0] = glm::vec3(1.0f, 0.3f, 0.0f);
	pickedColorArr[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	pickedColorArr[2] = glm::vec3(0.0f, 0.0f, 1.0f);

	
	
	//render loop
	while (!glfwWindowShouldClose(window)) { //as long as this windows closing flag is not set
		//if linking succeeded, we attach the shader executable to the rendering context:
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT );          // DONT FORGET TO RE-ENABLE DEPTH TESTING
		myShader.useProgram();
		//glBindVertexArray(vao);

		if (isToggled) { //render according to currently toggled cam location
			int idx = scene.getToggleIndex();
			struct camSnapshotData camData= *scene.getCamVec()[idx];
			cameraPos = camData.cameraPos;
			cameraFront = camData.camDirection;
		}

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 camSpaceTrans = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glm::frustum
		myShader.setMatrixUniform("camTransMatrix", camSpaceTrans);
		myShader.setMatrixUniform("projectionMatrix", projection);
		glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT); //cam view rendering

		scene.Draw(CAMERA_VIEW, myShader);
		//printf("drawn\n");

		if (pixelPicked) {               //draw to the pixel location that was picked by the mouse
			for (int i = 0; i < pickedPosVec.size();i++) {
				glm::uvec2 currPickedPos = pickedPosVec[i];
				glm::vec3 pickedColor = pickedColorArr[i];
				glEnable(GL_SCISSOR_TEST);
				glScissor(currPickedPos.x, currPickedPos.y, 4, 4);
				glClearColor(pickedColor.r, pickedColor.g, pickedColor.b, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glDisable(GL_SCISSOR_TEST);
			}
		}

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		camSpaceTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
		myShader.setMatrixUniform("camTransMatrix", camSpaceTrans);
		myShader.setMatrixUniform("projectionMatrix", projection);

		glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT); //global view rendering
		scene.Draw(GLOBAL_VIEW, myShader);

		glfwSwapBuffers(window); //swap buffers for the current contexted window
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;

}

void mouse_picking_callback(GLFWwindow* window, int button, int action, int mods) {	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xCoord = 0, yCoord = 0;
		glfwGetCursorPos(window, &xCoord, &yCoord); //here we query the recent cursor position (x,y) coords
		pickingX = (int)xCoord;
		pickingY = (int)yCoord;
		if (pickingX < 0 || pickingY < 0) {
			printf("Negative window coord.. something is wrong here");
			return;
		}
		else if (pickingX <= SCR_WIDTH / 2) {
			printf("Can pick only in cam view for now!");
			return;
		}

		glReadBuffer(GL_FRONT); //YOU SHOULD CHECK WHAT IS THE CORRECT TARGET OF THE COLOR BUFFER TO READ FROM

		float color_chans[3];
		glReadPixels(xCoord, yCoord, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_FLOAT, color_chans);
		printf("The picked pixel colors are: r=%c , g=%c, b=%c", color_chans[0], color_chans[1], color_chans[2]);
		glReadBuffer(0);
		//which framebuffer to read from??
		//glReadBuffer

	}


}

void rotate_cam_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_RIGHT: //perform yaw - clockwise say 6 degrees or if toggle is enabled, move between the camera poses
			if (isToggled) {
				scene.incToggleIndex();
			}
			else {
				yaw += 6.0f;
			}

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
		case GLFW_KEY_T:
			isToggled = !isToggled; //flip toggle status
			if (isToggled) {
				oldCameraPos = cameraPos;
				oldCameraFront = cameraFront;
			}
			else { //toggle mode is disabled - restore the oldest camera config in cam-view before toggle was initiated.
				cameraPos = oldCameraPos;
				oldCameraFront = cameraFront;
			}
			scene.flipToggleState();
		}
		
		

	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}


void mouse_cursor_picking_callback(GLFWwindow* window, double pickingX, double pickingY) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (pickingX < 0 || pickingY < 0) {
			printf("Negative window coord.. something is wrong here");
			return;
		}
		else if (pickingX <= SCR_WIDTH / 2) {
			printf("Can pick only in cam view for now!");
			return;
		}

		//glReadBuffer(GL_FRONT); //YOU SHOULD CHECK WHAT IS THE CORRECT TARGET OF THE COLOR BUFFER TO READ FROM

		unsigned char color_chans[4];
		int pixX = (int)pickingX;
		int pixY = (int)pickingY;
		glReadPixels(pixX, SCR_HEIGHT - pixY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, color_chans);
		printf("The picked pixel colors are: r=%d , g=%d, b=%d", (int)color_chans[0], (int)color_chans[1], (int)color_chans[2]);
		
		pixelPicked = true;
		pickedPos.x = pixX;
		pickedPos.y = SCR_HEIGHT - pixY;

		pickedPosVec.push_back(glm::uvec2(pickedPos.x, pickedPos.y));
		
		picked++;
		if (picked > 3) {
			picked = 0;
			pixelPicked = false;
			return; //pick exactly 3 - for now..
		}
		
		//glReadBuffer(0);
		//which framebuffer to read from??
		//glReadBuffer

	}
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