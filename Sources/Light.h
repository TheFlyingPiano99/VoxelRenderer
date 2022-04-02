#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

struct Light {
	glm::vec4 position;
	glm::vec3 powerDensity;
	glm::mat4 viewProjMatrix;

	void exportData(Shader& shader, int id) {
		std::string structName("lights[");
		structName.append(std::to_string(id)).append("]");
		std::string positionName(structName);
		positionName.append(".position");
		glUniform4f(glGetUniformLocation(shader.ID, positionName.c_str()), position.x, position.y, position.z, position.w);

		std::string powerDensityName(structName);
		powerDensityName.append(".powerDensity");
		glUniform3f(glGetUniformLocation(shader.ID, powerDensityName.c_str()), powerDensity.r, powerDensity.g, powerDensity.b);

		std::string viewProjName(structName);
		viewProjName.append(".viewProjMatrix");
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, viewProjName.c_str()), 1, GL_FALSE, glm::value_ptr(viewProjMatrix));
	}
};

