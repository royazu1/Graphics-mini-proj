#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

struct poseEstimationData {
	glm::vec3 camRotation;
	glm::vec3 camTranslation;
};

class PoseEstSolver
{
private:
	std::vector<cv::Point3d> poseVec3D;
	std::vector<cv::Point2d> poseVec2D;
	double focal_x;
	double focal_y;
	cv::Matx33d intrinsicMat;
	double c_x; //principal point u coord
	double c_y; //principal point v coord
	float window_height;

public:
	bool addPose(glm::vec3 pos); //need to reflect the y axis 
	bool addPose(glm::vec2 pos);	//need to reflect the y and z axes
	PoseEstSolver(int window_width, int window_height, float vertical_fov, float near);
	PoseEstSolver operator=(const PoseEstSolver& other);
	struct poseEstimationData solve(); //return the rotation/translation vectors based on picked 2D/3D 
	bool shouldSolve();
	void clear();
};

