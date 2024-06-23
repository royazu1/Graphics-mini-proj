#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define MAX_FOV_DEG 70.0f
#define MIN_FOV_DEG 10.0f
class Camera
{
private:
	float near;
	float far;
	//float horizontal_fov;
	float vertical_fov; //fov angle in degrees, will be converted to radians for calculations
	float aspect_ratio;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
public:
	Camera(float near, float far, float vertical_distance, float aspect_ratio, glm::vec3 camFront, glm::vec3 camPos); //calculate 
	glm::mat4 getProjectionMatrix();
	void modifyFOV(float degrees_delta); //degrees delta can be either positive or negative
	void setFOV(float newFOV);
	void setCamPos(glm::vec3 newCamPos);
	void setCamFront(glm::vec3 newCamFront);
	glm::vec3 getCamPos();
	glm::vec3 getCamFront();
	glm::mat4 getModelViewMat();
	float getFOV();
	void normalizeFront();
};

