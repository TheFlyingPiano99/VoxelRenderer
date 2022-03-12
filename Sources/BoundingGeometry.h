#pragma once
#include "VAO.h"
#include "Camera.h"
#include "Texture3D.h"
#include "Light.h"
#include "Texture1D.h"

class BoundingGeometry
{
	Shader* shader;
	VAO VAO;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	glm::mat4 modelMatrix;
	float threshold;

	void addCuboid(glm::vec3 scale, glm::vec3 translation);
	void calculateDivision(const Dimensions& dimensions, unsigned int& xDivision, unsigned int& yDivision, unsigned int& zDivision);
	void createVertexGrid(const Dimensions& dimensions, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void calculateFilled(const Dimensions& dimensions,
		const unsigned int& xDivision,
		const unsigned int& yDivision,
		const unsigned int& zDivision,
		bool* isFilled,
		Texture3D& voxels,
		Texture1D& transferFunction);
	void createIndices(const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision, bool* isFilled);

	void addPlusZSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void addMinusZSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void addPlusYSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void addMinusYSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void addPlusXSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void addMinusXSide(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);

	unsigned int indexVertices(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	unsigned int indexDivisionSized(const int x, const int y, const int z, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
public:
	BoundingGeometry(Shader* _shader)
		: shader(_shader), modelMatrix(1.0f) {
	}
	~BoundingGeometry() {
		VAO.Delete();
	}

	void updateGeometry(Texture3D& voxels, Texture1D& transferFunction, float threshold);

	void draw(Camera& camera, Light& light, glm::mat4& modelMatrix, glm::mat4& invModelMatrix, unsigned int enterFBO, unsigned int exitFBO, unsigned int lightFBO);
};

