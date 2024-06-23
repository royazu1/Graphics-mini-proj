#pragma once
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <glad/glad.h>
#include "Scene.h"
#include "stb_image.h"


//these headers are for mathemtical calculations (albeit for simple things you can use arrays of float and use a simple math library..)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>



class Map
{
private:
		unsigned char* img;
		int width;
		int height;
		int numChannels;
		float normalizeCoord(unsigned char pix_coord);
		struct Vertex** meshVertexData;
		float minHeight;
		float maxHeight;
		

		
public:
	Map(const char* img_path);
	unsigned int getIntensity(int y, int x);
	float** getHeightMap();


	void createMesh(int res, Scene & scene);
	void create3DMeshData();
	void cleanMeshData();
};

