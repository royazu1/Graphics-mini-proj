#include "PoseEstSolver.h"

bool PoseEstSolver::addPose(glm::vec3 pos)
{
	if (poseVec3D.size() == 3)
		return false;

	poseVec3D.push_back(cv::Point3d(pos.x, -pos.y, -pos.z)); //reflect the y,z axes - because of openCV reflected right handed coordinate system
	return true;
}

bool PoseEstSolver::addPose(glm::vec2 pos)
{
	if (poseVec3D.size() == 3)
		return false;
	poseVec2D.push_back(cv::Point2d(pos.x, -pos.y)); //reflect the y,z axes - because of openCV reflected right handed coordinate system
	return true;
}



PoseEstSolver::PoseEstSolver( int window_width,  int window_height, float bottom, float top, float right, float left, float near)
{
	focal_x =((float)window_width / (right - left)  ) * near;
	focal_y =((float)window_height / (top - bottom) ) * near;
	c_x = (double) window_width / 2;
	c_y = (double)window_height / 2;
	intrinsicMat = cv::Matx33d({ focal_x, 0 ,      c_x,
								0,     focal_y,   c_y,
								0,     0,         1 });
	std::cout << "IntrinsicMat = " << std::endl << " " << intrinsicMat << std::endl << std::endl;


}
