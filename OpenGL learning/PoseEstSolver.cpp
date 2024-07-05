#include "PoseEstSolver.h"

bool PoseEstSolver::addPose(glm::vec3 pos)
{
	if (poseVec3D.size() == 4)
		return false;

	poseVec3D.push_back(cv::Point3d(pos.x, -pos.y, -pos.z)); //reflect the y,z axes - because of openCV reflected right handed coordinate system
	return true;
}

bool PoseEstSolver::addPose(glm::vec2 pos)
{
	if (poseVec3D.size() == 4)
		return false;
	poseVec2D.push_back(cv::Point2d(pos.x, window_height - pos.y)); //reflect the y,z axes - because of openCV reflected right handed coordinate system
	return true;
}



PoseEstSolver::PoseEstSolver(int window_width, int window_height, float vertical_fov, float near)
{
	//calculations for symmetrical frustum that is calculated deciding the vertical FOV and aspect ratio to measure TOP and RIGHT planes
	float aspect = (float)window_width / (float)window_height;
	float top = glm::tan(glm::radians(vertical_fov / 2)) * near;
	float right = top * aspect;
	focal_x = (((float)window_width) / (right * 2)) * near; //near=f
	focal_y = (((float)window_height) / (top * 2)) * near;
	this->window_height = (float)window_height;
	c_x = (double)window_width / 2;
	c_y = (double)window_height / 2;
	intrinsicMat = cv::Matx33d({ focal_x, 0 ,      c_x,
								0,     focal_y,   c_y,
								0,     0,         1 });
	std::cout << "IntrinsicMat = " << std::endl << " " << intrinsicMat << std::endl << std::endl;

}

PoseEstSolver PoseEstSolver::operator=(const PoseEstSolver& other)
{
	if (this == &other) {
		return *this; // Check for self-assignment
	}

	// Copy each member
	poseVec3D = other.poseVec3D;
	poseVec2D = other.poseVec2D;
	focal_x = other.focal_x;
	focal_y = other.focal_y;
	intrinsicMat = other.intrinsicMat;
	c_x = other.c_x;
	c_y = other.c_y;
	window_height = other.window_height;

	return *this;
}

poseEstimationData PoseEstSolver::solve()
{
	std::vector<cv::Vec3d> solution;
	solution.push_back(cv::Vec3d(0, 0, 0)); //rvec
	solution.push_back(cv::Vec3d(0, 0, 0)); //tvec
	cv::Vec3d tvec = cv::Vec3d(0, 0, 0);
	cv::Vec3d rvec = cv::Vec3d(0, 0, 0);
	cv::Vec4d distCoeffs = cv::Vec4d(0, 0, 0, 0);
	try
	{
		//cv::solvePnP(poseVec3D, poseVec2D, intrinsicMat, distCoeffs, solution[0], solution[1], false, cv::SOLVEPNP_SQPNP);
		cv::solvePnP(poseVec3D, poseVec2D, intrinsicMat, distCoeffs, solution[0], solution[1], true, 0);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	struct poseEstimationData sol;
	cv::Matx33d rotMat;         //row order?
	cv::Rodrigues(solution[0], rotMat); //rodrigues from rvec
	cv::Matx31d camFront = rotMat.col(2);
	cv::Matx33d rotMatInv;
	cv::Vec3d campos = -rotMat.t() * solution[1];
	std::cout << "world to cam translation: = " << std::endl << " " << solution[1] << std::endl << std::endl;
	cv::Matx31d world_Zero = solution[1].val[0] * rotMat.col(0) + solution[1].val[1] * rotMat.col(1) + solution[1].val[2] * rotMat.col(2);
	std::cout << "world zero: = " << std::endl << " " << world_Zero << std::endl << std::endl;
	std::cout << "rodrigues= " << std::endl << " " << rotMat << std::endl << std::endl;
	std::cout << "rodrigues TP = " << std::endl << " " << rotMat.t() << std::endl << std::endl;
	std::cout << "rodrigues TP-cam front: = " << std::endl << " " << rotMat.t().col(2) << std::endl << std::endl;
	cv::transpose(rotMat, rotMatInv);
	cv::Vec3d invRotVec;
	cv::Rodrigues(rotMatInv, invRotVec);
	//sol.camRotation = glm::normalize(glm::vec3(solution[0].val[0], -solution[0].val[1], -solution[0].val[2]));
	sol.camRotation = glm::normalize(glm::vec3(-camFront.val[0], camFront.val[1], -camFront.val[2]));
	//sol.camRotation = glm::normalize(glm::vec3(rotMat.col(1), -rotMat.col(1), -rotMat.col(1)));
	sol.camTranslation = glm::vec3(campos.val[0], -campos.val[1], -campos.val[2]);
	//printf("SolvePnp pos= (x,y,z)=(%f,%f,%f)\n", campos.val[0], campos.val[1], campos.val[2]);
	//sol.camTranslation = glm::vec3(solution[1].val[0], -solution[1].val[1], -solution[1].val[2]);
	return sol;
}

bool PoseEstSolver::shouldSolve()
{
	return poseVec2D.size() == 4 && poseVec3D.size() == 4;
}

void PoseEstSolver::clear()
{
	this->poseVec2D.clear();
	this->poseVec3D.clear();
}


