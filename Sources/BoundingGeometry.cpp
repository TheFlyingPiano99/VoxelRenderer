#include "BoundingGeometry.h"
#include"EBO.h"
#include "VBO.h"
#include<glm/gtc/type_ptr.hpp>

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

void BoundingGeometry::calculateDivision(const Dimensions& dimensions, unsigned int& xDivision, unsigned int& yDivision, unsigned int& zDivision)
{
	for (unsigned int i = 32; i >= 1; i--) {
		if (dimensions.width % i == 0) {
			xDivision = i;
			break;
		}
	}
	for (unsigned int i = 32; i >= 1; i--) {
		if (dimensions.height % i == 0) {
			yDivision = i;
			break;
		}
	}
	for (unsigned int i = 32; i >= 1; i--) {
		if (dimensions.depth % i == 0) {
			zDivision = i;
			break;
		}
	}
}

void BoundingGeometry::createVertexGrid(const Dimensions& dimensions, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	glm::vec3 blockSize = glm::vec3(dimensions.width, dimensions.height, dimensions.depth)
		/ glm::vec3(xDivision, yDivision, zDivision);
	for (int z = 0; z < zDivision + 1; z++) {
		for (int y = 0; y < yDivision + 1; y++) {
			for (int x = 0; x < xDivision + 1; x++) {
				vertices.push_back(glm::vec3(x, y, z) * blockSize);
			}
		}
	}
}


void BoundingGeometry::addPlusZSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	//Lower polygon:
	indices.push_back(indexVertices(x+1, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z+1, xDivision, yDivision, zDivision));

	//Upper polygon:
	indices.push_back(indexVertices(x+1, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z+1, xDivision, yDivision, zDivision));
}

void BoundingGeometry::addMinusZSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	//Lower polygon:
	indices.push_back(indexVertices(x, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z, xDivision, yDivision, zDivision));

	//Upper polygon:
	indices.push_back(indexVertices(x, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z, xDivision, yDivision, zDivision));
}

void BoundingGeometry::addPlusYSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	//Lower polygon:
	indices.push_back(indexVertices(x, y+1, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z+1, xDivision, yDivision, zDivision));

	//Upper polygon:
	indices.push_back(indexVertices(x, y+1, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z+1, xDivision, yDivision, zDivision));
}

void BoundingGeometry::addMinusYSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	//Lower polygon:
	indices.push_back(indexVertices(x, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y, z, xDivision, yDivision, zDivision));

	//Upper polygon:
	indices.push_back(indexVertices(x, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y, z+1, xDivision, yDivision, zDivision));
}

void BoundingGeometry::addPlusXSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	//Lower polygon:
	indices.push_back(indexVertices(x+1, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));

	//Upper polygon:
	indices.push_back(indexVertices(x+1, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x + 1, y + 1, z + 1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x+1, y+1, z, xDivision, yDivision, zDivision));
}

void BoundingGeometry::addMinusXSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	//Lower polygon:
	indices.push_back(indexVertices(x, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z, xDivision, yDivision, zDivision));

	//Upper polygon:
	indices.push_back(indexVertices(x, y, z+1, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z, xDivision, yDivision, zDivision));
	indices.push_back(indexVertices(x, y+1, z+1, xDivision, yDivision, zDivision));
}

unsigned int BoundingGeometry::indexVertices(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{
	return z * (yDivision + 1) * (xDivision + 1) + y * (xDivision + 1) + x;
}

unsigned int BoundingGeometry::indexDivisionSized(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision)
{

	return z * yDivision * xDivision + y * xDivision + x;
}

void BoundingGeometry::calculateFilled(const Dimensions& dimensions,
	const unsigned int& xDivision,
	const unsigned int& yDivision,
	const unsigned int& zDivision,
	bool* isFilled,
	Texture3D& voxels,
	TransferFunction& transferFunction)
{
	std::vector<float> averageOpacity = std::vector<float>(xDivision * yDivision * zDivision);
	for (int i = 0; i < xDivision * yDivision * zDivision; i++) {
		averageOpacity[i] = 0.0f;
	}
	int xBlockSize = dimensions.width / xDivision;
	int yBlockSize = dimensions.height / yDivision;
	int zBlockSize = dimensions.depth / zDivision;
	int voxelsPerBlock = xBlockSize * yBlockSize * zBlockSize;
	for (int z = 0; z < dimensions.depth; z++) {
		if (z % 5 == 0) {
			std::cout << "Completion: " << z / (float)dimensions.depth * 100.0f << "%" << std::endl;
		}
		for (int y = 0; y < dimensions.height; y++) {
			for (int x = 0; x < dimensions.width; x++) {
				glm::vec4 gradientIntensity = voxels.resampleGradientAndDensity(glm::ivec3(x, y, z));
				float gradientLength = glm::length(glm::vec3(gradientIntensity.x, gradientIntensity.y, gradientIntensity.z));
				float intensity = gradientIntensity.w;
				float opacity = transferFunction(glm::vec2(intensity, gradientLength)).w;
					averageOpacity[indexDivisionSized((x / xBlockSize), (y / yBlockSize), (z / zBlockSize), xDivision, yDivision, zDivision)]
					+= opacity / (float)voxelsPerBlock;
			}
		}
	}
	for (int i = 0; i < xDivision * yDivision * zDivision; i++) {
		isFilled[i] = (threshold <= averageOpacity[i]);
	}
}

void BoundingGeometry::createIndices(const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision, bool* isFilled)
{
	for (int z = 0; z < zDivision; z++) {
		for (int y = 0; y < yDivision; y++) {
			for (int x = 0; x < xDivision; x++) {
				if (isFilled[z * yDivision * xDivision + y * xDivision + x]) {
					if (z < zDivision - 1 && !isFilled[(z + 1) * yDivision * xDivision + y * xDivision + x]
						|| z == zDivision - 1) {
						addPlusZSide(x, y, z, xDivision, yDivision,zDivision);
					}
					if (z > 0 && !isFilled[(z - 1) * yDivision * xDivision + y * xDivision + x]
						|| z == 0) {
						addMinusZSide(x, y, z, xDivision, yDivision, zDivision);
					}
					if (y < yDivision - 1 && !isFilled[z * yDivision * xDivision + (y + 1) * xDivision + x]
						|| y == yDivision - 1) {
						addPlusYSide(x, y, z, xDivision, yDivision, zDivision);
					}
					if (y > 0 && !isFilled[z * yDivision * xDivision + (y - 1) * xDivision + x]
						|| y == 0) {
						addMinusYSide(x, y, z, xDivision, yDivision, zDivision);
					}
					if (x < xDivision - 1 && !isFilled[z * yDivision * xDivision + y * xDivision + x + 1]
						|| x == xDivision - 1) {
						addPlusXSide(x, y, z, xDivision, yDivision, zDivision);
					}
					if (x > 0 && !isFilled[z * yDivision * xDivision + y * xDivision + x - 1]
						|| x == 0) {
						addMinusXSide(x, y, z, xDivision, yDivision, zDivision);
					}
				}
			}
		}
	};
}


void BoundingGeometry::updateGeometry(Texture3D& voxels, TransferFunction& transferFunction, float threshold)
{
	std::cout << "Updating bounding geometry." << std::endl;
	this->threshold = threshold;
	vertices.clear();
	indices.clear();

	Dimensions dimensions = voxels.getDimensions();
	unsigned int xDivision, yDivision, zDivision;
	calculateDivision(dimensions, xDivision, yDivision, zDivision);
	bool* isFilled = new bool[xDivision * yDivision * zDivision];

	calculateFilled(dimensions,
		xDivision,
		yDivision,
		zDivision,
		isFilled,
		voxels,
		transferFunction);

	createVertexGrid(dimensions, xDivision, yDivision, zDivision);
	createIndices(xDivision, yDivision, zDivision, isFilled);

	delete[] isFilled;

	VAO.Bind();
	VBO VBO(vertices);
	EBO EBO(indices);
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
	VAO.Unbind();
	std::cout << "Bounding geometry is ready." << std::endl;
}

void BoundingGeometry::draw(Camera& camera, Light& light, glm::mat4& modelMatrix, glm::mat4& invModelMatrix, unsigned int enterFBO, unsigned int exitFBO, unsigned int lightFBO)
{
	shader->Activate();
	VAO.Bind();
	camera.exportMatrix(*shader);
	glm::vec4 modelSpaceCameraPos = invModelMatrix * glm::vec4(camera.eye, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glBindFramebuffer(GL_FRAMEBUFFER, exitFBO);
	glClearColor(modelSpaceCameraPos.x, modelSpaceCameraPos.y, modelSpaceCameraPos.z, 0);
	glClearDepth(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	
	glBindFramebuffer(GL_FRAMEBUFFER, enterFBO);
	glClearColor(modelSpaceCameraPos.x, modelSpaceCameraPos.y, modelSpaceCameraPos.z, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glm::vec4 modelSpaceLightPos = invModelMatrix * glm::vec4(light.position, 1.0f);
	glClearColor(modelSpaceLightPos.x, modelSpaceLightPos.y, modelSpaceLightPos.z, 0);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "camera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(light.viewProjMatrix));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
