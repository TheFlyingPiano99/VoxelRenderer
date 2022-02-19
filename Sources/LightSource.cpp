#include "LightSource.h"


void PointLight::exportData(Shader* shader) {
	shader->Activate();
	std::string structName = std::string("pointLights[").append(std::to_string(index)).append("].");
	std::string posName = structName;
	posName.append("position");
	std::string constName = structName;
	constName.append("constant");
	std::string linearName = structName;
	linearName.append("linear");
	std::string quadraticName = structName;
	quadraticName.append("quadratic");
	std::string ambientName = structName;
	ambientName.append("ambient");
	std::string diffName = structName;
	diffName.append("diffuse");
	std::string specName = structName;
	specName.append("specular");

	glUniform3f(glGetUniformLocation(shader->ID, posName.c_str()), position.x, position.y, position.z);
	glUniform1f(glGetUniformLocation(shader->ID, constName.c_str()), constant);
	glUniform1f(glGetUniformLocation(shader->ID, linearName.c_str()), linear);
	glUniform1f(glGetUniformLocation(shader->ID, quadraticName.c_str()), quadratic);
	glUniform3f(glGetUniformLocation(shader->ID, ambientName.c_str()), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shader->ID, diffName.c_str()), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shader->ID, specName.c_str()), specular.x, specular.y, specular.z);
}

void DirectionalLight::exportData(Shader* shader)
{
	shader->Activate();
	glUniform3f(glGetUniformLocation(shader->ID, "dirLight.direction"), direction.x, direction.y, direction.z);
	glUniform3f(glGetUniformLocation(shader->ID, "dirLight.ambient"), ambient.x, ambient.y, ambient.z);
	glUniform3f(glGetUniformLocation(shader->ID, "dirLight.diffuse"), diffuse.x, diffuse.y, diffuse.z);
	glUniform3f(glGetUniformLocation(shader->ID, "dirLight.specular"), specular.x, specular.y, specular.z);
}
