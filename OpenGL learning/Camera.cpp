#include "Camera.h"

Camera::Camera(float near, float far, float vertical_fov,float aspect_ratio, glm::vec3 camFront, glm::vec3 camPos)
{
	this->near = near;
	this->far = far;
	this->aspect_ratio = aspect_ratio;
	this->vertical_fov = vertical_fov; //in DEGREES 
}

glm::mat4 Camera::getProjectionMatrix()
{
	glm::mat4 proj = glm::perspective(glm::radians(vertical_fov), aspect_ratio, near, far);
	return proj;
}

void Camera::modifyFOV(float degrees_delta)
{
	this->vertical_fov += degrees_delta;
	if (vertical_fov > MAX_FOV_DEG) {
		vertical_fov = MAX_FOV_DEG;
	}
	if (vertical_fov < MIN_FOV_DEG) {
		vertical_fov = MIN_FOV_DEG;
	}
}

void Camera::setFOV(float newFOV)
{
	vertical_fov = newFOV;
}

void Camera::setCamPos(glm::vec3 newCamPos)
{
	cameraPos = newCamPos;
}

void Camera::setCamFront(glm::vec3 newCamFront)
{
	cameraFront = newCamFront;
}

glm::vec3 Camera::getCamPos()
{
	return cameraPos;
}

glm::vec3 Camera::getCamFront()
{
	return cameraFront;
}

glm::mat4 Camera::getModelViewMat() {
	glm::vec3 fakeUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 modelView = glm::lookAt(cameraPos, cameraPos+cameraFront, fakeUp);
	return modelView;
}

float Camera::getFOV()
{
	return vertical_fov;
}

void Camera::normalizeFrom()
{
	cameraFront = glm::normalize(cameraFront);
}
