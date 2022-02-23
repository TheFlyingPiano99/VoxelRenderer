#include "VoxelData.h"


void VoxelData::exportData()
{
	glUniform1f(glGetUniformLocation(shader->ID, "maxIntensity"), 
		maxIntensity);
	glUniform1f(glGetUniformLocation(shader->ID, "maxAttenuation"),
		maxAttenuation);
}

VoxelData::VoxelData(Shader* _shader, const char* file)
		: shader(_shader), maxIntensity(255), maxAttenuation(255)
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
	
	voxels = new Texture3D(file, 0, GL_RED, GL_UNSIGNED_BYTE);
	unsigned char* bytes = new unsigned char[100 * 4];
	for (int i = 0; i < 100; i++) {
		if (i < 5) {
			bytes[i * 4] = 0;
			bytes[i * 4 + 1] = 0;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 0;
		}
		else if (i < 40) {
			bytes[i * 4] = 10;
			bytes[i * 4 + 1] = 0;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 3;
		}
		else {
			bytes[i * 4] = 0;
			bytes[i * 4 + 1] = 50;
			bytes[i * 4 + 2] = 0;
			bytes[i * 4 + 3] = 5;
		}
	}
	transfer = new Texture1D(bytes, 100, 1, GL_RGBA, GL_UNSIGNED_BYTE);
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
	transfer->Delete();
	delete transfer;
}

void VoxelData::draw(Camera& camera) {
	shader->Activate();
	voxels->Bind();
	transfer->Bind();
	this->exportData();
	camera.exportData(*shader);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	voxels->Unbind();
	transfer->Unbind();
}
