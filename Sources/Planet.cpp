#include "Planet.h"

#include <vector>

#include "DualNumber.h"
#include "MathUtil.h"


Planet::Planet(Shader* _shader) : SceneObject(nullptr, _shader) {
	std::vector<glm::vec3> colors;
	colors.push_back(this->colors[0]);
	colors.push_back(this->colors[1]);
	colors.push_back(this->colors[2]);
	colors.push_back(this->colors[3]);
	this->mesh = Mesh::createSphere(planetRadius, colors);
	atmosphere.center = position;
	atmosphere.radius = 6.0f;

	atmosphere.quadraticAbsorption = glm::vec3(0.2, 5.0, 6.0);
	atmosphere.linearAbsorption = glm::vec3(1, 1, 1);
	atmosphere.constantAbsorption = glm::vec3(1, 1, 1);

	atmosphere.quadraticScattering = glm::vec3(0.14, 0.15, 0.15);
	atmosphere.linearScattering = glm::vec3(0.001, 0.1, 0.4);
	atmosphere.constantScattering = glm::vec3(0.0, 0.5, 1);

	atmosphere.quadraticReflectiveness = glm::vec3(0.8, 0.8, 0.8);
	atmosphere.linearReflectiveness = glm::vec3(0.1, 0.1, 0.8);
	atmosphere.constantReflectiveness = glm::vec3(1, 1, 1);

	atmosphere.quadraticDensity = 0.1f;
	atmosphere.linearDensity = 0.5f;
	atmosphere.constantDensity = 0.0f;
}

void Planet::exportAtmosphere(Shader& shader) {
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.center"), atmosphere.center.x, atmosphere.center.y, atmosphere.center.z);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.radius"), atmosphere.radius);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.planetRadius"), planetRadius);

	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.rayleighScattering"), atmosphere.rayleighScattering.x, atmosphere.rayleighScattering.y, atmosphere.rayleighScattering.z);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.mieScattering"), atmosphere.mieScattering);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.heightOfAverageDensity"), atmosphere.heightOfAverageDensity);


	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.quadraticAbsorption"), atmosphere.quadraticAbsorption.x, atmosphere.quadraticAbsorption.y, atmosphere.quadraticAbsorption.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.linearAbsorption"), atmosphere.linearAbsorption.x, atmosphere.linearAbsorption.y, atmosphere.linearAbsorption.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.constantAbsorption"), atmosphere.constantAbsorption.x, atmosphere.constantAbsorption.y, atmosphere.constantAbsorption.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.quadraticScattering"), atmosphere.quadraticScattering.x, atmosphere.quadraticScattering.y, atmosphere.quadraticScattering.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.linearScattering"), atmosphere.linearScattering.x, atmosphere.linearScattering.y, atmosphere.linearScattering.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.constantScattering"), atmosphere.constantScattering.x, atmosphere.constantScattering.y, atmosphere.constantScattering.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.quadratiReflectiveness"), atmosphere.quadraticReflectiveness.x, atmosphere.quadraticReflectiveness.y, atmosphere.quadraticReflectiveness.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.linearReflectiveness"), atmosphere.linearReflectiveness.x, atmosphere.linearReflectiveness.y, atmosphere.linearReflectiveness.z);
	glUniform3f(glGetUniformLocation(shader.ID, "atmosphere.constantReflectiveness"), atmosphere.constantReflectiveness.x, atmosphere.constantReflectiveness.y, atmosphere.constantReflectiveness.z);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.quadratiDensity"), atmosphere.quadraticDensity);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.linearDensity"), atmosphere.linearDensity);
	glUniform1f(glGetUniformLocation(shader.ID, "atmosphere.constantDensity"), atmosphere.constantDensity);
}

glm::vec3* Planet::getRayleighScattering()
{
	return &atmosphere.rayleighScattering;
}

float* Planet::getMieScattering()
{
	return &atmosphere.mieScattering;
}

float* Planet::getQuadraticDensity()
{
	return &(atmosphere.quadraticDensity);
}

float* Planet::getLinearDensity()
{
	return &(atmosphere.linearDensity);
}

float* Planet::getConstantDensity()
{
	return &(atmosphere.constantDensity);
}


glm::vec3* Planet::getQuadraticAbsorption()
{
	return &(atmosphere.quadraticAbsorption);
}

glm::vec3* Planet::getLinearAbsorption()
{
	return &(atmosphere.linearAbsorption);
}

glm::vec3* Planet::getConstantAbsorption()
{
	return &(atmosphere.constantAbsorption);
}


glm::vec3* Planet::getQuadraticScattering()
{
	return &(atmosphere.quadraticScattering);
}

glm::vec3* Planet::getLinearScattering()
{
	return &(atmosphere.linearScattering);
}

glm::vec3* Planet::getConstantScattering()
{
	return &(atmosphere.constantScattering);
}


glm::vec3* Planet::getQuadraticReflectiveness()
{
	return &(atmosphere.quadraticReflectiveness);
}

glm::vec3* Planet::getLinearReflectiveness()
{
	return &(atmosphere.linearReflectiveness);
}

glm::vec3* Planet::getConstantReflectiveness()
{
	return &(atmosphere.constantReflectiveness);
}

float Planet::getRadius() {
	return planetRadius;
}
