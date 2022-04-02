#include "SceneObject.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>

inline void SceneObject::update() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(position);
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
		glUniform1i(glGetUniformLocation(shader->ID, "lightCount"), lights.size());
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		mesh->Draw(*shader, camera);
	}
}

void SceneObject::exportMatrix()
{
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
}
