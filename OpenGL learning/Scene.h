#pragma once
//#include "Camera.h"

#include "stb_image.h"
#include <string>
#include <vector>
#include <glad/glad.h>

typedef std::string str;

class Scene
{

public:
	Scene();
	//void load2DMap(str filePath);
	//void makeHeightMap();
	//void setUpSplitScrean();
	void Draw();
	//void record();
	void addVAOconfig(unsigned int vaoRef); //add a VAO object ref to the vao_vec vector, to be used at render call
private:
	//Camera cam;
	std::vector<unsigned int> VAO_vec;
	int vecSize;
};

