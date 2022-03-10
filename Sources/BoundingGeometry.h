#pragma once
#include "VAO.h"
#include "Camera.h"
#include "Texture3D.h"

class BoundingGeometry
{
	Shader* shader;
	VAO VAO;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	glm::mat4 modelMatrix;

	void addCuboid(glm::vec3 scale, glm::vec3 translation);

public:
	BoundingGeometry(Shader* _shader) 
		: shader(_shader), modelMatrix(1.0f) {
	}
	~BoundingGeometry() {
		VAO.Delete();
	}

	void updateGeometry(Texture3D& voxels);

	void draw(Camera& camera, glm::mat4& modelMatrix, unsigned int enterFBO, unsigned int exitFBO);
};

