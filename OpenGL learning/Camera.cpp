#include "Camera.h"

Camera::Camera(float near, float far, float vertical_distance, float horizontal_distance)
{
	this->near = near;
	//this->aspect_ratio = horizontal_distance / vertical_distance; //horizontal is already equal to the aspect_ratio.. remove later
	this->vertical_fov = 2 * glm::atan((vertical_distance / near)); //in radians 
	//projectionMat = glm::perspective(vertical_fov, horizontal_distance, near, far);
	projectionMat = glm::frustum(-horizontal_distance, horizontal_distance, -vertical_distance, vertical_distance, near, far);
}

glm::mat4& Camera::getProjectionMatrix()
{
	return projectionMat;
}
