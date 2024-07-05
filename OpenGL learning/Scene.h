#pragma once
//#include "Camera.h"

#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <string>
#include <vector>
#include <glad/glad.h>
#include <set>
#include "Camera.h"

//these headers are for mathemtical calculations (albeit for simple things you can use arrays of float and use a simple math library..)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

#include "Shader.h"
#include "PoseEstSolver.h"
#include <map>
typedef std::string str;


struct camSnapshotData {
	glm::vec3 cameraPos;
	glm::vec3 camDirection;
	float fov;
};

struct Vertex {
	float vCoords[3];
	float vColor[3];
	//	float normal[3];
};


enum viewTypes
{
	GLOBAL_VIEW = 0,
	CAMERA_VIEW = 1
};


class Scene
{

public:
	Scene();
	//void load2DMap(str filePath);
	//void makeHeightMap();
	//void setUpSplitScrean();
	void Draw(int type, Shader & drawingShader);
	//void record();
	void addVAOconfig(unsigned int vaoRef); //add a VAO object ref to the vao_vec vector, to be used at render call
	void addMarkerVAO(unsigned int vaoRef);
	void addCamPosRenderVAO(glm::vec3 &cameraPos, glm::vec3& cameraFrontVec, float fov);
	void computeCamPose(PoseEstSolver slv, int width, int height, std::vector<glm::uvec2>& pickedPos, Camera& viewCam,float fov);
	void incToggleIndex();
	void decToggleIndex();
	void flipToggleState();
	std::vector<struct camSnapshotData*> getCamVec();
	std::vector<struct camSnapshotData*> getCompPoseVec();
	int getToggleIndex();
	void addMarker(const float c, const glm::vec3 p);//green color ,position
	void flipCamPosRender();

private:
	//Camera cam;
	std::vector<unsigned int> VAO_vec;
	std::vector<unsigned int> camRenderVAOs;
	std::vector<unsigned int> markerVAOs;
	int vecSize;
	void storeVertexData(struct Vertex* vertex, glm::vec3 pos, glm::vec3 color);
	bool isToggled;
	int toggleIndex;
	//Shader toggleShader;
	std::vector<struct camSnapshotData*> camSnapshotsVec;
	std::vector<struct camSnapshotData*> computedCamPoseVec;
	std::map<unsigned char,glm::vec3> markers;
	bool renderCamPos; //
};

