#include "Scene.h"


Scene::Scene() {
	vecSize = 0;
}

void Scene::Draw()
{
	for (int i = 0; i < VAO_vec.size();i++) {
		unsigned int currVAO = VAO_vec[i];
		glBindVertexArray(currVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL); //alternatively use the same index array that you define in the cpu for all VAO's to save memory
		glBindVertexArray(0);
	}


}

void Scene::addVAOconfig(unsigned int vaoRef)
{
	this->VAO_vec.push_back(vaoRef);
	vecSize++;
}
