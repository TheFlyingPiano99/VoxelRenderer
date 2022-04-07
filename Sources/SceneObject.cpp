#include "SceneObject.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>

void SceneObject::update() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(position) 
		* glm::rotate(eulerAngles.x, glm::vec3(1,0,0)) 
		* glm::rotate(eulerAngles.y, glm::vec3(0, 1, 0)) 
		* glm::rotate(eulerAngles.z, glm::vec3(0, 0, 1))
		* glm::scale(scale);
	invModelMatrix = glm::inverse(modelMatrix);
}

void SceneObject::animate(float dt)
{
	if (nullptr != animation) {
		animation->perform(this, dt);
		update();
	}
}

void SceneObject::draw(Camera& camera, std::vector<Light>& lights)
{
	if (nullptr != mesh && nullptr != shader) {
		shader->Activate();
		exportMatrix();
		camera.exportData(*shader);
		for (int i = 0; i < lights.size(); i++) {
			lights[i].exportData(*shader, i);
		}
		glUniform1ui(glGetUniformLocation(shader->ID, "lightCount"), lights.size());
		glUniform1f(glGetUniformLocation(shader->ID, "material.shininess"), 20.0f);
		glUniform3f(glGetUniformLocation(shader->ID, "material.specularColor"), 1.0f, 1.0f, 1.0f);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDepthFunc(GL_LESS);
		mesh->Draw(*shader, camera);
	}
}

void SceneObject::exportMatrix()
{
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
}
