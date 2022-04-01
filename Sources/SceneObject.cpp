#include "SceneObject.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>

inline void SceneObject::updateMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(position);
	invModelMatrix = glm::inverse(modelMatrix);
}

void SceneObject::animate(float dt)
{
	if (nullptr != animation) {
		animation->perform(this, dt);
		updateMatrix();
	}
}

void SceneObject::draw(Camera& camera)
{
	if (nullptr != mesh && nullptr != shader) {
		shader->Activate();
		exportMatrix();
		camera.exportData(*shader);
		if (nullptr != light) {
			light->exportData(shader);
		}
		mesh->Draw(*shader, camera);
	}
}

void SceneObject::exportMatrix()
{
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
}
