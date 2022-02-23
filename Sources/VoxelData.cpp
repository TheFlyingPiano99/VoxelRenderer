#include "VoxelData.h"


void VoxelData::exportData()
{
	glUniform3f(glGetUniformLocation(shader->ID, "resolution"), 
		resolution.x ,resolution.y, resolution.z);
	glUniform3f(glGetUniformLocation(shader->ID, "light1.direction"),
		lightDir.x, lightDir.y, lightDir.z);
	glUniform1f(glGetUniformLocation(shader->ID, "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader->ID, "gamma"), gamma);
}

unsigned char* VoxelData::defaultTransferFunction(int resolution)
{
	unsigned char* bytes = new unsigned char[resolution * 4];
	for (int i = 0; i < resolution; i++) {
		if (i < 2 / 100.0 * resolution) {
			bytes[i * 4] = 0;
			bytes[i * 4 + 1] = 0;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 0;
		}
		else if (i < 3 / 100.0 * resolution) {
			bytes[i * 4] = 5;
			bytes[i * 4 + 1] = 2;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 5;
		}
		else if (i < 4 / 100.0 * resolution) {
			bytes[i * 4] = 2;
			bytes[i * 4 + 1] = 10;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 5;
		}
		else if (i < 5 / 100.0 * resolution) {
			bytes[i * 4] = 120;
			bytes[i * 4 + 1] = 92;
			bytes[i * 4 + 2] = 80;
			bytes[i * 4 + 3] = 5;
		}
		else if (i < 10 / 100.0 * resolution) {
			bytes[i * 4] = 135;
			bytes[i * 4 + 1] = 103;
			bytes[i * 4 + 2] = 90;
			bytes[i * 4 + 3] = 100;
		}
		else if (i < 20 / 100.0 * resolution) {
			bytes[i * 4] = 165;
			bytes[i * 4 + 1] = 57;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 100;
		}
		else if (i < 30 / 100.0 * resolution) {
			bytes[i * 4] = 210;
			bytes[i * 4 + 1] = 161;
			bytes[i * 4 + 2] = 141;
			bytes[i * 4 + 3] = 100;
		}
		else if (i < 40 / 100.0 * resolution) {
			bytes[i * 4] = 225;
			bytes[i * 4 + 1] = 172;
			bytes[i * 4 + 2] = 150;
			bytes[i * 4 + 3] = 150;
		}
		else if (i < 50 / 100.0 * resolution) {
			bytes[i * 4] = 240;
			bytes[i * 4 + 1] = 184;
			bytes[i * 4 + 2] = 160;
			bytes[i * 4 + 3] = 200;
		}
		else if (i < 80 / 100.0 * resolution) {
			bytes[i * 4] = 100;
			bytes[i * 4 + 1] = 100;
			bytes[i * 4 + 2] = 100;
			bytes[i * 4 + 3] = 250;
		}
		else {
			bytes[i * 4] = 0;
			bytes[i * 4 + 1] = 0;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 0;
		}
	}
	return bytes;
}

unsigned char* VoxelData::brainOnly(int resolution)
{
	unsigned char* bytes = new unsigned char[resolution * 4];
	for (int i = 0; i < resolution; i++) {
		if (i > 0.4 * resolution && i < 0.5 * resolution) {
			bytes[i * 4] = 200;
			bytes[i * 4 + 1] = 200;
			bytes[i * 4 + 2] = 150;
			bytes[i * 4 + 3] = 100;

		}
		else if (i > 0.5 * resolution && i < 1.0 * resolution) {
			bytes[i * 4] = 200;
			bytes[i * 4 + 1] = 200;
			bytes[i * 4 + 2] = 200;
			bytes[i * 4 + 3] = 200;

		}
		else {
			bytes[i * 4] = 0;
			bytes[i * 4 + 1] = 0;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 0;
		}
	}
	return bytes;
}

VoxelData::VoxelData(Shader* _shader, const char* file)
		: shader(_shader),
	maxIntensity(255),
	maxAttenuation(255),
	resolution(256, 256, 99),
	plane(glm::vec3(100,100,50), glm::vec3(0,0,1)),
	exposure(1.1f),
	gamma(0.98f),
	lightDir(glm::normalize(glm::vec3(1,1,1)))
	{
	float quadVertices[] =
	{
		//Coord	//texCoords
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f,

		1.0f,  1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f
	};
	unsigned char* bytes = defaultTransferFunction(256);
	//unsigned char* bytes = brainOnly(256);
	voxels = new Texture3D(file, 0, GL_RED, GL_UNSIGNED_BYTE);
	transferFunction = new Texture1D(bytes, 256, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	delete[] bytes;

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

VoxelData::~VoxelData() {
	glBindVertexArray(quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteVertexArrays(1, &quadVAO);
	voxels->Delete();
	delete voxels;
	transferFunction->Delete();
	delete transferFunction;
}

void VoxelData::animate(float dt)
{
	glm::mat4 M(1);
	M = glm::rotate(M, dt * 0.001f, glm::vec3(0, 1, 0));
	glm::vec4 rotated = M * glm::vec4(lightDir, 1);
	lightDir = rotated;
}

void VoxelData::draw(Camera& camera) {
	shader->Activate();
	voxels->Bind();
	transferFunction->Bind();
	this->exportData();
	camera.exportData(*shader);
	plane.exportData(shader, "intersectionPlane");
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
	voxels->Unbind();
	transferFunction->Unbind();
}

void VoxelData::shiftIntersectionPlane(float delta)
{
	plane.setPoint(plane.getPoint() + delta * plane.getNormal());
}

void VoxelData::rotateIntersectionPlane(float rad)
{
	glm::mat4 M(1);
	M = glm::rotate(M, rad, glm::vec3(0, 1, 0));
	glm::vec4 rotated = M * glm::vec4(plane.getNormal(), 1);
	plane.setNormal(rotated);
}
