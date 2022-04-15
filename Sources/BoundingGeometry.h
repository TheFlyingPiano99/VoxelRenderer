#pragma once
#include "VAO.h"
#include "Camera.h"
#include "Texture3D.h"
#include "Light.h"
#include "Texture2D.h"
#include "TransferFunction.h"
#include "FBO.h"

class BoundingGeometry
{
	Shader* modelPosShader = nullptr;
	Shader* flatColorShader = nullptr;
	VAO VAO;
	std::vector<glm::vec3> vertices;
	std::vector<GLuint> indices;
	float threshold;
	glm::vec4 flatColor;

	void addCuboid(glm::vec3 scale, glm::vec3 translation);
	void calculateDivision(const Dimensions& dimensions, unsigned int& xDivision, unsigned int& yDivision, unsigned int& zDivision);
	void createVertexGrid(const Dimensions& dimensions, const unsigned int& xDivision, const unsigned int& yDivision, const unsigned int& zDivision);
	void calculateFilled(const Dimensions& dimensions,
		const unsigned int& xDivision,
		const unsigned int& yDivision,
		const unsigned int& zDivision,
		bool* isFilled,
		Texture3D& voxelTexture,
		TransferFunction& transferFunction);
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
	BoundingGeometry(Shader* _modelPosShader, Shader* _flatColorShader)
		: modelPosShader(_modelPosShader), flatColorShader(_flatColorShader), threshold(0.01f) {
	}
	~BoundingGeometry() {
		VAO.Delete();
	}

	void updateGeometry(Texture3D& voxelTexture, TransferFunction& transferFunction, float threshold);

	void draw(Camera& camera, std::vector<Light>& lights, glm::mat4& modelMatrix, glm::mat4& invModelMatrix, FBO& enterFBO, FBO& exitFBO, FBO* lightFBOs);
	void drawOnScreen(FBO& fbo, Camera& camera, glm::mat4& modelMatrix, glm::mat4& invModelMatrix, float opacity);
};

