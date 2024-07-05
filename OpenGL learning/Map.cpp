#include "Map.h"
#include "Scene.h"
#include <stdlib.h> 




float Map::normalizeCoord(unsigned char pix_coord) //map pix coord to the normalized [-1,1] interval
{
	
	return -1.0f + ((float)pix_coord / (( (float)width - 1.0f) * 2.0f));
}

Map::Map(const char* img_path)
{
	img = stbi_load(img_path, &width, &height, &numChannels, 1); //need to check if loaded succesfully
	if (!img) {
		std::cout << "Map image wasn't loaded succesfully!" << std::endl;
		std::cout << stbi_failure_reason() << std::endl;
	}
	minHeight = 255.0f;
	maxHeight = 0.0f;
	meshVertexData = NULL;
}

unsigned int Map::getIntensity(int y, int x)
{
	return (unsigned int)img[(width * y) + x];
}

float** Map::getHeightMap()
{
	float currentHeight = 0;

	float** heightMap = new float *[height];
	for (int y = 0; y < height;y++) {
		heightMap[y] = new float[width];
		for (int x = 0; x < width; x++) {
			heightMap[y][x] = (float) getIntensity(y, x);
			currentHeight = heightMap[y][x];
			if (currentHeight > maxHeight) {
				maxHeight = currentHeight;
			}
			if (currentHeight < minHeight) {
				minHeight = currentHeight;
			}
		}
	}



	for (int y = 0; y < height;y++) {
		for (int x = 0; x < height;x++) {
		//	heightMap[y][x] = (heightMap[y][x] - minHeight) * f;
		}
	}

	return heightMap;
}

void Map::createMesh(int res, Scene& scene)
{
	//debug
	
	//debug
	unsigned int currVAO;
	unsigned int currVBO;
	unsigned int currEBO;
	int renderIndexes [] = {0,1,2,
							0,2,3 };
	glGenBuffers(1, &currEBO);

	int yRes = height / 30; //height / far_plane_dist
	int xRes = width / 30;

	//debug

	for (int y = 0; y < height - yRes; y+= yRes) {
		for (int x = 0; x < width - xRes; x+=xRes) {
			struct Vertex currVertexData[4];           //data for the current 4 vertices that define a square of adjacent pixels 2 pix wide and long
			currVertexData[0] = meshVertexData[y][x]; //top left
			currVertexData[1] = meshVertexData[y+yRes][x]; //bottom left
			currVertexData[2] = meshVertexData[y+yRes][x+xRes]; //bottom right
			currVertexData[3] = meshVertexData[y][x+xRes]; //top right

			glGenVertexArrays(1, &currVAO);
			glBindVertexArray(currVAO);
		
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(renderIndexes), renderIndexes, GL_STATIC_DRAW);

			glGenBuffers(1,&currVBO);
			glBindBuffer(GL_ARRAY_BUFFER, currVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(currVertexData), currVertexData, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3 , GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void *) 0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*) (3 * sizeof(float))); //remember the last arg is offset in bytes!
			glEnableVertexAttribArray(1);
			/*
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);
			*/
		


			//UNBIND ALL CONFIGURATIONS TO NOT MIX IN EACH SQUARE VERTICES CONFIGURATION THAT IS SENT TO THE GPU
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(0);
			scene.addVAOconfig(currVAO);

		}
	}

	for (int i = 0; i < 50; i++) {
		glGenVertexArrays(1, &currVAO);
		glBindVertexArray(currVAO);
		glGenBuffers(1, &currVBO);
		glBindBuffer(GL_ARRAY_BUFFER, currVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(markerVertexData[i]), &markerVertexData[i], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)(3 * sizeof(float))); //remember the last arg is offset in bytes!
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		scene.addMarkerVAO(currVAO);


	}
	
	//free vertex memory
	cleanMeshData();
}

void Map::create3DMeshData(Scene& scene)
{
	float currentHeight = 0;
	meshVertexData = new struct Vertex*[height];
	markerVertexData = new struct Vertex[50];

	float** heightMap = getHeightMap();
	
	float maxX = (float)width - 1;
	float maxY = (float)height - 1;

	
	
	int yRes = height / 5; //height / far_plane_dist
	int xRes = width / 10;
	float jump = 1 / 64.0 ;
	float curGreen = jump;
	int markerIdx = 0;//debug
	for (int y = 0; y < height;y++) {
		meshVertexData[y] = new struct Vertex[width];
		for (int x = 0; x < width;x++) {
			float xCoord= -20.0f + ((float)x * 40.0f / maxX);  //map x axis to world's x axis 
			float zCoord = -100.0f + ((float)y * (100.f -0.1f) / maxY); // map the y axis to world z axis , going from the far plane inwards  
			float yCoord = 0.0f + (heightMap[y][x] - minHeight) * 2.2f / (maxHeight - minHeight)  ; //map Z coord to the [-1,1] normalized range
			
			//add to the vertex array 
			//struct Vertex currVertex = meshVertexData[y][x];
			meshVertexData[y][x].vCoords[0] = xCoord;
			meshVertexData[y][x].vCoords[1] = yCoord;
			meshVertexData[y][x].vCoords[2] = zCoord;
			if (x% xRes== 0 && y% yRes ==0)
			{
				
				//meshVertexData[y][x].vColor[0] = 0;
				//meshVertexData[y][x].vColor[1] = curGreen;
				//meshVertexData[y][x].vColor[2] = 0;
				
				scene.addMarker((unsigned char)(roundf(curGreen*255)), glm::vec3(xCoord, yCoord+2.0, zCoord));
				std::cout << "add marker: green-color=" << (int)(curGreen * 255) << " mark-num "<< markerIdx << std::endl;
				std::cout << "Green= " << (int)roundf(curGreen * 255) << " x, y, z = " << xCoord << " ," << yCoord + 2.0f << " ," << zCoord << std::endl;
				markerVertexData[markerIdx].vCoords[0] = xCoord;
				markerVertexData[markerIdx].vCoords[1] = yCoord+2.0f;
				markerVertexData[markerIdx].vCoords[2] = zCoord;
				markerVertexData[markerIdx].vColor[0] = 0;
				markerVertexData[markerIdx].vColor[1] = curGreen;
				markerVertexData[markerIdx].vColor[2] = 0;
					
				markerIdx++;
				curGreen += jump;
					
			}
			meshVertexData[y][x].vColor[0] = (yCoord / 2.2f);
			meshVertexData[y][x].vColor[1] = 0;
			meshVertexData[y][x].vColor[2] = 1 - yCoord / 2.2f;

			//printf("current vertex: x=%f ,y=%f, z=%f\n", xCoord, yCoord, zCoord);
		}
	}
	printf("num markers=%d\n",markerIdx);
	//free z values array mem
	for (int y = 0; y < height;y++) {
		delete[] heightMap[y];
	}
	delete[] heightMap;
	//free z values array mem


}

void Map::cleanMeshData()
{
	for (int y = 0; y < height;y++) {
		delete meshVertexData[y];
	}
	delete meshVertexData;
	if (markerVertexData) {
		delete markerVertexData;
	}
}
