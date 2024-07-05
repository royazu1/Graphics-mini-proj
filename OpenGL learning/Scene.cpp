#include "Scene.h"
#include "stb_image_write.h"
#define RED 0
#define GREEN 1
#define BLUE 2

Scene::Scene() {
	vecSize = 0;
	toggleIndex = 0;
	renderCamPos = true;
	isToggled = false;
}

void Scene::Draw(int type, Shader & drawingShader) // 0 for global view, 1 for cam view
{
	
	for (int i = 0; i < VAO_vec.size();i++) {
		unsigned int currVAO = VAO_vec[i];
		glBindVertexArray(currVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL); //alternatively use the same index array that you define in the cpu for all VAO's to save memory
		glBindVertexArray(0);
	}

	glPointSize(3.0);//
	for (int j = 0; renderCamPos && j < camRenderVAOs.size() && type == GLOBAL_VIEW; j++) { //draw only in global view, and only when NOT in flight compute mode!
		unsigned int currCamPosVao = camRenderVAOs[j];
		glBindVertexArray(currCamPosVao);
		if (j != toggleIndex) {
			//glDrawArrays(GL_POINTS, 0, 3);
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
		else if (isToggled) {
			//printf("in scene: toggled is on!\n");
			drawingShader.setIntegerUniform("toggledFrag", 1);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			drawingShader.setIntegerUniform("toggledFrag", 0);
		}

		glBindVertexArray(0); //

	}

	for (int k = 0; k < markerVAOs.size();k++) {
		unsigned int currMarkerVao = markerVAOs[k];
		glBindVertexArray(currMarkerVao);
		glDrawArrays(GL_POINTS, 0, 1);
	}
}

void Scene::addVAOconfig(unsigned int vaoRef)
{
	this->VAO_vec.push_back(vaoRef);
	vecSize++;
}

void Scene::addMarkerVAO(unsigned int vaoRef)
{
	this->markerVAOs.push_back(vaoRef);
}

void Scene::addCamPosRenderVAO(glm::vec3 & cameraPos, glm::vec3 & cameraFrontVec,float fov) //make a pefect triangle with camera pos at the front vertex
{

	//store cam snapshot data in the vector
	struct camSnapshotData * currCamSnap= new struct camSnapshotData;
	currCamSnap->camDirection = cameraFrontVec;
	currCamSnap->cameraPos = cameraPos;
	currCamSnap->fov = fov;
	camSnapshotsVec.push_back(currCamSnap);
	//

	//x,y,z right handed coord system
	glm::vec3 fakeUpVec=glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rightVec = glm::cross(fakeUpVec, -cameraFrontVec); //right hand rule
	glm::vec3 realUpVec =glm::normalize( glm::cross(-cameraFrontVec, rightVec)); //right hand rule

	glm::vec4 toRightVerDir =glm::normalize( glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f), realUpVec) * glm::vec4(cameraFrontVec,1.0f) );
	glm::vec3 toRightPoint = glm::vec3(toRightVerDir);
	float mult =1.0f / glm::cos(glm::radians(30.0f));
	glm::vec3 rightVertexTriangle = cameraPos + (toRightPoint * mult);

	glm::vec4 toLeftVerDir = glm::normalize(glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), realUpVec) * glm::vec4(cameraFrontVec, 1.0f));
	glm::vec3 toLeftPoint = glm::vec3(toLeftVerDir);
	glm::vec3 leftVertexTriangle= cameraPos + (toLeftPoint * mult);



	glm::vec3 blueColorVec = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 greenColVec = glm::vec3(0.0f, 1.0f, 0.0f);
	struct Vertex camVertexData[3];
	storeVertexData(&camVertexData[0], cameraPos, blueColorVec);
	storeVertexData(&camVertexData[1], rightVertexTriangle, greenColVec);
	storeVertexData(&camVertexData[2], leftVertexTriangle, greenColVec);


	GLuint camVao;
	glGenVertexArrays(1, &camVao);
	glBindVertexArray(camVao);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(camVertexData), camVertexData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(struct Vertex), (void*) (3 *sizeof(float) ));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->camRenderVAOs.push_back(camVao);
}

void Scene::computeCamPose(PoseEstSolver slv,int width,int height, std::vector<glm::uvec2>& pickedPos, Camera& viewCam,float fov) //NOTE - width is the half-viewport width so no need to div by 2!
{
	std::set<unsigned char> chosenColorsSet;
	int count = 0;
	int maxGreen = 0;
	int bSize = width * height * 4; //4 chans rgba
	GLubyte* buffer = new GLubyte[bSize];
	glReadPixels(width, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	stbi_write_png("fml.png", width, height, 4, buffer, width  * 4);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			GLubyte red = buffer[(x*4) + (y * width* 4)];
			GLubyte green = buffer[(x*4) + GREEN + (y * width * 4)];
			GLubyte blue = buffer[(x*4) + BLUE + (y * width * 4)];
			//printf("pix: (x,y)=(%d,%d) , (r,g,b)=(%d,%d,%d) \n",x% (width/2), y, red, green, blue);

			
			if (slv.shouldSolve()) {
				printf("Found 4 markers! finding solution\n");
				goto Solve; //found 4 markers - enough for pnp-solve
			}

			bool notWhite = red != 255 && green != 255 && blue != 255;
			if (notWhite && red < 2 && blue < 2)
			{
			float jump = 4.0f;
			int fGreen = floor(green);
			float freq = green - fGreen;
			if (fGreen % 4 == 0) green = fGreen;
			else green = fGreen + (fGreen % 4);
			auto it = markers.find((unsigned char)roundf(green));

			if (it != markers.end() && chosenColorsSet.find(green) == chosenColorsSet.end()) {
				chosenColorsSet.insert(green);
				printf("found marker! Red=%d Green=%d Blue=%d 2D (x,y)= (%d,%d) , 3D (x,y,z)=(%f,%f,%f)\n", (int)red, (int)green, (int)blue, x, y, glm::vec3(it->second).x, glm::vec3(it->second).y, glm::vec3(it->second).z);
				slv.addPose(glm::vec2(x, y));
				//CHECK THE UNPROJECTION HERE!
				unsigned int depth = 0;
				unsigned int maxInt = (1 << 32) - 1;
				glReadPixels(x + width, y, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, &depth);
				glm::vec3 unproj_pixel = glm::unProjectNO(glm::vec3(x + width, y, (float)depth / (float)maxInt), viewCam.getModelViewMat(), viewCam.getProjectionMatrix(), glm::vec4(width, 0, width, height));
				printf("real pose pix: (x,y,z)=(%f,%f,%f)\n", unproj_pixel.x, unproj_pixel.y, unproj_pixel.z);
				//pickedPos.push_back(glm::uvec2(x, y));
				slv.addPose(glm::vec3(it->second)); //??
				break;
			}
			}
		}
	}
	printf("count=%d ,max=%d\n", count,maxGreen); //debug

Solve:
	camSnapshotData* compPose = new camSnapshotData;
	if (slv.shouldSolve()) {
		poseEstimationData sol = slv.solve();
		compPose->camDirection = sol.camRotation;
		compPose->cameraPos = sol.camTranslation;
		compPose->fov = fov;
		printf("Sol: camPos:(x,y,z)=(%f,%f,%f)\n", compPose->cameraPos.x, compPose->cameraPos.y, compPose->cameraPos.z);
		printf("Sol: camDir:(x,y,z)=(%f,%f,%f)\n", compPose->camDirection.x, compPose->camDirection.y, compPose->camDirection.z);
	}
	else { // found less than 4 markers - can't solve, maybe display in that case the background WHITE color..
		printf("Can't solve for this camera view pose! not enough markers from that view!\n");
		compPose->camDirection=glm::vec3(0, 0, 1); //look out of the screen so view will be WHITE
		compPose->cameraPos= glm::vec3(0, 0, 0); //
	}

	this->computedCamPoseVec.push_back(compPose);
}

void Scene::incToggleIndex()
{
	toggleIndex = (toggleIndex + 1) % camSnapshotsVec.size();
}

void Scene::decToggleIndex() {
	if (toggleIndex - 1 == 0) {
		toggleIndex = 0;
	}
	else {
		toggleIndex = (toggleIndex - 1) % camSnapshotsVec.size();
	}
	
}

void Scene::flipToggleState()
{
	isToggled = !isToggled;
}

std::vector<struct camSnapshotData*> Scene::getCamVec() {
	return this->camSnapshotsVec;
}

std::vector<struct camSnapshotData*> Scene::getCompPoseVec()
{
	return this->computedCamPoseVec;
}

int Scene::getToggleIndex()
{
	return toggleIndex;
}

void Scene::addMarker(const float c, const glm::vec3 p)
{
	markers[c] = p;
}

void Scene::flipCamPosRender()
{
	renderCamPos = !renderCamPos;
}

void Scene::storeVertexData(Vertex* vertex, glm::vec3 pos, glm::vec3 color)
{
	vertex->vCoords[0] = pos.x;
	vertex->vCoords[1] = pos.y;
	vertex->vCoords[2] = pos.z;
	vertex->vColor[0] = color.x;
	vertex->vColor[1] = color.y;
	vertex->vColor[2] = color.z;
}
