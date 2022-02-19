#include "Sun.h"

Sun::Sun(Shader* _shader) : SceneObject(nullptr, _shader) {
	mesh = Mesh::createSphere(0.2f, std::vector<glm::vec3>());
}

Sun::~Sun()
{
	if (lightCamera != nullptr) {
		delete lightCamera;
	}
}

void Sun::exportData(Shader& shader) {
	glUniform3f(glGetUniformLocation(shader.ID, "sun.position"), position.x, position.y, position.z);
	glm::vec3 color = light->getDiffuse();
	glUniform3f(glGetUniformLocation(shader.ID, "sun.color"), color.x, color.y, color.z);
}

void Sun::updateLightCamera(Planet& planet)
{
	if (lightCamera == nullptr) {
		lightCamera = new Camera(1024, 1024, position);
	}
	lightCamera->setPosition(position);
	lightCamera->Orientation = glm::normalize(planet.getPosition() - position);
	lightCamera->updateMatrix();
}

Camera* Sun::getLightCamera() {
	return lightCamera;
}
