#pragma once
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <glad/glad.h>
#include "Scene.h"
#include "stb_image.h"

struct Vertex {
	float vCoords[3];// x,y,z point
	float vColor[3];//r,g,b
	float normal[3];//x,y,z vec
};

class Map
{
private:
		unsigned char* img;
		int width;
		int height;
		int numChannels;
		float normalizeCoord(unsigned char pix_coord);
		struct Vertex** meshVertexData;
		

		
public:
	Map(const char* img_path);
	unsigned int getIntensity(int y, int x);
	float** getHeightMap();

	void createMesh(int res, Scene & scene);
	void create3DMeshData();
};

