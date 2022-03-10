#include "BoundingGeometry.h"
#include"EBO.h"
#include "VBO.h"

glm::vec3 cubeVertices[] =
{
	glm::vec3(0.0f, 0.0f,  1.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 0.0f,  1.0f),
	glm::vec3(0.0f,  1.0f,  1.0f),
	glm::vec3(0.0f,  1.0f, 0.0f),
	glm::vec3(1.0f,  1.0f, 0.0f),
	glm::vec3(1.0f,  1.0f,  1.0f)
};

GLuint cubeIndices[] =
{
	0, 2, 1,
	0, 3, 2,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

void BoundingGeometry::addCuboid(glm::vec3 scale, glm::vec3 translation) {
	int indexOffset = vertices.size();
	for (int i = 0; i < 8; i++) {
		vertices.push_back(cubeVertices[i] * scale + translation);
	}
	for (int i = 0; i < 36; i++) {
		indices.push_back(cubeIndices[i] + indexOffset);
	}
}

void BoundingGeometry::updateGeometry(Texture3D& voxels)
{
	Dimensions dimensions = voxels.getDimensions();

	vertices.clear();
	indices.clear();

	addCuboid(glm::vec3(dimensions.width, dimensions.height, dimensions.depth), glm::vec3(0.0f));

	//---------------------------------------------------------------------------------

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices

	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
}

void BoundingGeometry::draw(Camera& camera, glm::mat4& modelMatrix, unsigned int enterFBO, unsigned int exitFBO)
{
	shader->Activate();
	VAO.Bind();
	camera.exportMatrix(*shader);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glBindFramebuffer(GL_FRAMEBUFFER, enterFBO);
	glClearColor(camera.Position.x, camera.Position.y, camera.Position.z, 0);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, exitFBO);
	glClearColor(camera.Position.x, camera.Position.y, camera.Position.z, 0);
	glClearDepth(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}
