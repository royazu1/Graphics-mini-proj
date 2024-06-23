#include "Scene.h"




Scene::Scene() {
	vecSize = 0;
	toggleIndex = 0;
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

	//glPointSize(10.0);//
	for (int j = 0; j < camRenderVAOs.size() && type == GLOBAL_VIEW; j++) { //draw only in global view
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
}

void Scene::addVAOconfig(unsigned int vaoRef)
{
	this->VAO_vec.push_back(vaoRef);
	vecSize++;
}

void Scene::addCamPosRenderVAO(glm::vec3 & cameraPos, glm::vec3 & cameraFrontVec) //make a pefect triangle with camera pos at the front vertex
{

	//store cam snapshot data in the vector
	struct camSnapshotData * currCamSnap= new struct camSnapshotData;
	currCamSnap->camDirection = cameraFrontVec;
	currCamSnap->cameraPos = cameraPos;
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

int Scene::getToggleIndex()
{
	return toggleIndex;
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
