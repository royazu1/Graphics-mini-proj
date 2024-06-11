#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

class PoseEstSolver
{
private:
	std::vector<cv::Point3d> poseVec3D;
	std::vector<cv::Point2d> poseVec2D;
	float focal_x;
	float focal_y;
	cv::Matx33d intrinsicMat;
	double c_x; //principal point u coord
	double c_y; //principal point v coord

public:
	bool addPose(glm::vec3 pos); //need to reflect the y axis 
	bool addPose(glm::vec2 pos);	//need to reflect the y and z axes
	PoseEstSolver(int window_width, int window_height, float bottom, float top, float right, float left, float near);
};

