
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <glut/include/GL/glut.h>
#include "stb_image.h"
#include "Shader.h"
#include "Map.h"
#include "Camera.h"
//these headers are for mathemtical calculations (albeit for simple things you can use arrays of float and use a simple math library..)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>




void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keypress_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void rotate_cam_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_picking_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_cursor_picking_callback(GLFWwindow* window, double xposIn, double yposIn);
void cursor_pos_cv(GLFWwindow* window, double pickingX, double pickingY);

#define DEF_MOD 0
#define FLIGHT_MOD 10
#define REC_MOD 15
#define PLAYBACK_MOD 20
#define PICKING_MOD 25
#define DIFF_MOD 30

// settings
 int SCR_WIDTH = 800;
 int SCR_HEIGHT = 600;
 int MODE = DEF_MOD;
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 oldCameraPos; //this is to backup the camera-pos before we entered toggle mode
glm::vec3 oldCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);


bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)600.0 / 2.0;
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
glm::uvec2 pickedPos;    //holds the picked pixel coords 
int picked = 0;
float aspect_ratio = (float)SCR_WIDTH * 0.5 / (float)SCR_HEIGHT;
//PoseEstSolver mySolver(SCR_WIDTH/2, SCR_HEIGHT, -0.0577f, 0.0577f, aspect_ratio,-aspect_ratio,0.1f);
PoseEstSolver mySolver(SCR_WIDTH / 2, SCR_HEIGHT, -0.02f, 0.02f, 0.05f, -0.05f, 0.1f);
//glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
//Camera camera(0.1f, 50.0f, 0.0577f, aspect_ratio);
Camera camera(0.1f, 50.0f, 0.02f, 0.05f);


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
	glfwSetMouseButtonCallback(window, mouse_picking_callback);
	glfwSetCursorPosCallback(window, cursor_pos_cv);

	//glfwSetCursorPosCallback(window, mouse_cursor_picking_callback);
	Shader myShader = Shader("testVertexShader.vs", "testFragShader.fs");
	

	
	Map myMap = Map("heatmap.jpg");

	myMap.create3DMeshData();
	myMap.createMesh(0,scene); //RES is unusued for now
	
	glm::vec3 pickedColorArr[3];
	pickedColorArr[0] = glm::vec3(1.0f, 1.0f, 0.0f);
	pickedColorArr[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	pickedColorArr[2] = glm::vec3(0.0f, 0.0f, 1.0f);

	
	//enable depth testing - check the effect of reading from the depth buffer
	glEnable(GL_DEPTH_TEST);
	//render loop
	while (!glfwWindowShouldClose(window)) { //as long as this windows closing flag is not set
		//if linking succeeded, we attach the shader executable to the rendering context:
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);          // DONT FORGET TO RE-ENABLE DEPTH TESTING
		
		myShader.useProgram();
		//glBindVertexArray(vao);

		if (isToggled || MODE == PLAYBACK_MOD) { //render according to currently toggled cam location
			int idx = scene.getToggleIndex();
			struct camSnapshotData camData= *scene.getCamVec()[idx];
			cameraPos = camData.cameraPos;
			cameraFront = camData.camDirection;
		}

		//glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 1.0f, 100.0f);
		glm::mat4 projection = camera.getProjectionMatrix();
		glm::mat4 camSpaceTrans = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//glm::frustum
		myShader.setMatrixUniform("camTransMatrix", camSpaceTrans);
		myShader.setMatrixUniform("projectionMatrix", projection);
		glViewport(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT); //cam view rendering

		scene.Draw(CAMERA_VIEW, myShader);
		//printf("drawn\n");

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		if (MODE == DIFF_MOD)
		{
			poseEstimationData ped = mySolver.solve();
			cameraPos = ped.camTranslation;
			cameraFront = ped.camRotation;
			camSpaceTrans = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		}
		else {
			camSpaceTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		}
		myShader.setMatrixUniform("camTransMatrix", camSpaceTrans);
		myShader.setMatrixUniform("projectionMatrix", projection);

		glViewport(0, 0, SCR_WIDTH / 2, SCR_HEIGHT); //global view rendering
		scene.Draw(GLOBAL_VIEW, myShader);
		
		if (pixelPicked /*&& PICKING_MOD*/) { //draw to the pixel location that was picked by the mouse - should be done right before swapping color buffs
			for (int i = 0; i < pickedPosVec.size();i++) {
				glm::uvec2 currPickedPos = pickedPosVec[i];
				glm::vec3 pickedColor = pickedColorArr[i];
				glEnable(GL_SCISSOR_TEST);
				glScissor(currPickedPos.x, currPickedPos.y, 6, 6);
				glClearColor(pickedColor.r, pickedColor.g, pickedColor.b, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);
				glDisable(GL_SCISSOR_TEST);
			}
		}
		glfwSwapBuffers(window); //swap buffers for the current contexted window
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void mouse_picking_callback(GLFWwindow* window, int button, int action, int mods) {	
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && MODE == PICKING_MOD) {
		pickingX = (int)lastX;
		pickingY = (int) SCR_HEIGHT - lastY;
		if (lastX < 0 || lastY < 0) {
			printf("Negative window coord.. something is wrong here");
			return;
		}
		else if (pickingX <= SCR_WIDTH / 2) {
			printf("Picking in global view...\n");
			//read from the depth-buffer, then call glm::unproject to attain point at world coordinates, call addPose with the 3d pose vec
			GLuint depth_value;
			glReadPixels(pickingX, pickingY, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, &depth_value); //?? for some reason z-value is the same for all pixels
			GLuint max_int = (1 << 32) - 1;
			glm::vec3 pose_NDC = glm::vec3(pickingX, pickingY, (float)depth_value / (float)max_int);
			printf("depth=%u (normalized to 32 bit integer depth), and =%f\n",depth_value, (float)depth_value / (float) max_int);
			glm::vec4 viewport_vec = glm::vec4(0, 0, SCR_WIDTH / 2, SCR_HEIGHT);
			glm::mat4 globModelViewMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
			glm::vec3 pose_3D = glm::unProjectNO(pose_NDC, globModelViewMat, camera.getProjectionMatrix(), viewport_vec);
			printf("pose3d: (x,y,z)=(%f ,%f ,%f)\n", pose_3D.x, pose_3D.y,pose_3D.z);
			mySolver.addPose(pose_3D);
		}
		else { // pickingX > SCR_WIDTH / 2      -- picking in cam view , use addPose with a 2d vec
				printf("Picking in cam view...\n");
			printf("pose2d: (u,v)=(%f ,%f)\n", pickingX - (float)(SCR_WIDTH / 2), (float)pickingY);
			mySolver.addPose(glm::vec2(pickingX - (float)(SCR_WIDTH/2), (float)pickingY));
			// debug
			/*
			
			GLuint depth_value;
			glReadPixels(pickingX, pickingY, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, &depth_value); //?? for some reason z-value is the same for all pixels
			GLuint max_int = (1 << 32) - 1;
			glm::vec3 pose_NDC = glm::vec3(pickingX, pickingY, (float)depth_value / (float)max_int);
			printf("depth=%u (normalized to 32 bit integer depth), and =%f\n", depth_value, (float)depth_value / (float)max_int);
			glm::vec4 viewport_vec = glm::vec4(SCR_WIDTH / 2, 0, SCR_WIDTH / 2, SCR_HEIGHT);
			glm::mat4 globModelViewMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			glm::vec3 pose_3D = glm::unProjectNO(pose_NDC, globModelViewMat, camera.getProjectionMatrix(), viewport_vec);
			printf("pose3d: (x,y,z)=(%f ,%f ,%f)\n", pose_3D.x, pose_3D.y, pose_3D.z);
			mySolver.addPose(pose_3D);
			
			
			*/
		}
		
		pickedPosVec.push_back(glm::uvec2(pickingX, pickingY));
		pixelPicked = true;
		picked++;
		
		//NO NEED ANYMORE!!
		/*if (mySolver.shouldSolve()) {
			struct poseEstimationData sol = mySolver.solve();
			printf("sol-camPos: (x,y,z)=(%f,%f,%f)\n", sol.camTranslation.x, sol.camTranslation.y, sol.camTranslation.z);
			printf("sol-camDir: (x,y,z)=(%f,%f,%f)\n", sol.camRotation.x, sol.camRotation.y, sol.camRotation.z);
			scene.addCamPosRenderVAO(sol.camTranslation, sol.camRotation);
		}*/
	}


}
void cursor_pos_cv(GLFWwindow* window, double pickingX, double pickingY) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS) { //update the pos only when we are not picking
		lastX = pickingX;
		lastY = pickingY;
	}
	
}

void rotate_cam_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		int modif = glutGetModifiers();//GLUT_ACTIVE_SHIFT == modif
		switch (key) {
		case GLFW_KEY_RIGHT: //perform yaw - clockwise say 6 degrees, or if toggle is enabled, move between the camera poses
			if (MODE == PLAYBACK_MOD) {
				scene.incToggleIndex();
			}
			else {
				yaw += 6.0f;
			}
		//	if (yaw >= 89.0f) { yaw = 89.0f; }
			break;
		case GLFW_KEY_LEFT: //perform yaw - counter-clockwise
			if (MODE == PLAYBACK_MOD) {
				scene.decToggleIndex();
			}
			else {
				yaw -= 6.0f;
			}
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
			//printf("camPos: (x,y,z)=(%f,%f,%f)\n", cameraPos.x,cameraPos.y,cameraPos.z);
			//printf("camDir: (x,y,z)=(%f,%f,%f)\n", cameraFront.x, cameraFront.y, cameraFront.z);
			//scene.addCamPosRenderVAO(cameraPos, cameraFront);
			if (GLUT_ACTIVE_SHIFT == modif ) { // Togle between poses
				MODE = PLAYBACK_MOD;
			}
			else {
				MODE = REC_MOD;
			}
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
			break;
		case GLFW_KEY_B:
			if(MODE == REC_MOD){
				scene.addCamPosRenderVAO(cameraPos, cameraFront);
			}
			break;
		case GLFW_KEY_P:
			if (MODE == PICKING_MOD)
			{
				MODE = DEF_MOD;
			}else
				MODE = PICKING_MOD;
			break;
		case GLFW_KEY_C:
			if (mySolver.shouldSolve())
			{
				MODE = DIFF_MOD;
			}
			else
			{
				std::cout << "can't solve cam pos, not enough points\n";
			}
			break;
		}
	}

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

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

