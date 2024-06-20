#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
private:
	float near;
	float far;
	float horizontal_fov;
	float vertical_fov;
	float aspect_ratio;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::mat4 projectionMat;
public:
	Camera(float near, float far, float vertical_distance, float horizontal_distance); //calculate 
	glm::mat4& getProjectionMatrix();
};

