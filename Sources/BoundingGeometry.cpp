#include "BoundingGeometry.h"
#include"EBO.h"
#include "VBO.h"

void BoundingGeometry::updateGeometry(Texture3D& voxels)
{
	Dimensions dimensions = voxels.getDimensions();

	glm::mat4 scale = glm::scale(glm::vec3(dimensions.width, dimensions.height, dimensions.depth));
	//glm::mat4 translate = glm::translate(glm::vec3(dimensions.width / 2.0f, dimensions.height / 2.0f, dimensions.depth / 2.0f));
	modelMatrix = scale;

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
	vertices.clear();
	indices.clear();
	vertices.insert(vertices.begin(), std::begin(cubeVertices), std::end(cubeVertices));
	indices.insert(indices.begin(), std::begin(cubeIndices), std::end(cubeIndices));

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

void BoundingGeometry::draw(Camera& camera, unsigned int enterFBO, unsigned int exitFBO)
{
	shader->Activate();
	VAO.Bind();
	camera.exportMatrix(*shader);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glBindFramebuffer(GL_FRAMEBUFFER, enterFBO);
	glClearColor(camera.Position.x, camera.Position.y, camera.Position.z, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, exitFBO);
	glClearColor(camera.Position.x, camera.Position.y, camera.Position.z, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_BACK);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}
