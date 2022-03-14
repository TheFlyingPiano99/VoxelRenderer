#include "SceneObject.h"
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>

inline void SceneObject::updateMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(position);
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
		exportMatrix("model");
		if (light != nullptr) {
			glm::vec3 color = light->getDiffuse();
			glUniform3f(glGetUniformLocation(shader->ID, "lightColor"), color.x, color.y, color.z);
		}
		mesh->Draw(*shader, camera);
	}
}

void SceneObject::exportMatrix(const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, uniform), 1, GL_FALSE, glm::value_ptr(modelMatrix));
}
