#pragma once
//#include "Camera.h"

#include "stb_image.h"
#include <string>
#include <vector>
#include <glad/glad.h>

//these headers are for mathemtical calculations (albeit for simple things you can use arrays of float and use a simple math library..)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>



typedef std::string str;



struct Vertex {
	float vCoords[3];
	float vColor[3];
	//	float normal[3];
};

class Scene
{

public:
	Scene();
	//void load2DMap(str filePath);
	//void makeHeightMap();
	//void setUpSplitScrean();
	void Draw(int type);
	//void record();
	void addVAOconfig(unsigned int vaoRef); //add a VAO object ref to the vao_vec vector, to be used at render call
	void addCamPosRenderVAO(glm::vec3 &cameraPos, glm::vec3& cameraFrontVec);

private:
	//Camera cam;
	std::vector<unsigned int> VAO_vec;
	std::vector<unsigned int> camRenderVAOs;
	int vecSize;
	void storeVertexData(struct Vertex* vertex, glm::vec3 pos, glm::vec3 color);
};

