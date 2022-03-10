#include "VoxelData.h"
#include <vector>


void VoxelData::exportData()
{
	glUniform3f(glGetUniformLocation(shader->ID, "resolution"),
		resolution.x ,resolution.y, resolution.z);
	glUniform3f(glGetUniformLocation(shader->ID, "light1.position"),
		lightPosition.x, lightPosition.y, lightPosition.z);
	glUniform3f(glGetUniformLocation(shader->ID, "light1.intensity"),
		lightIntensity.x, lightIntensity.y, lightIntensity.z);
	glUniform1f(glGetUniformLocation(shader->ID, "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader->ID, "gamma"), gamma);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));

	glUniform1ui(glGetUniformLocation(shader->ID, "shadowSamples"), shadowSamples);
}

unsigned char* VoxelData::skinTransferFunction(int resolution)
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

unsigned char* VoxelData::defaultTransferFunction(int resolution)
{
	unsigned char* bytes = new unsigned char[resolution * 4];
	for (int i = 0; i < resolution; i++) {
		if (i > 3) {
			bytes[i * 4] = i / (float)resolution * 256.0f;
			bytes[i * 4 + 1] = i / (float)resolution * 256.0f * i / 256.0f;
			bytes[i * 4 + 2] = i / (float)resolution * 256.0f * i / 256.0f;
			bytes[i * 4 + 3] = (std::pow(i, 0.5) <= 256.0f)? std::pow(i, 0.5)  / (float)resolution * 256.0f : 256.0f;
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

unsigned char* VoxelData::brainOnlyTransferFunction(int resolution)
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

bool VoxelData::readDimensions(const char* path, std::string& name, Dimensions& dimensions)
{
	std::ifstream dimesionsStream;
	std::string line;
	std::string token;
	std::vector<std::string> tokens;
	dimesionsStream.open(path);
	if (!dimesionsStream) {
		return false;
	}
	else {
		while (std::getline(dimesionsStream, line)) {
			std::stringstream lineStream = std::stringstream(line);
			while (std::getline(lineStream, token, ' ')) {
				tokens.push_back(token);
				if (tokens.size() >= 3 && tokens[tokens.size() - 2] == "=") {
					if (tokens[tokens.size() - 3] == "name") {
						name = tokens[tokens.size() - 1];
					}
					else if (tokens[tokens.size() - 3] == "width") {
						dimensions.width = std::stoi(tokens[tokens.size() - 1]);
					}
					else if (tokens[tokens.size() - 3] == "height") {
						dimensions.height = std::stoi(tokens[tokens.size() - 1]);
					}
					else if (tokens[tokens.size() - 3] == "depth") {
						dimensions.depth = std::stoi(tokens[tokens.size() - 1]);
					}
					else if (tokens[tokens.size() - 3] == "bytesPerVoxel") {
						dimensions.bytesPerVoxel = std::stoi(tokens[tokens.size() - 1]);
					}
					else if (tokens[tokens.size() - 3] == "widthScale") {
						dimensions.widthScale = std::stof(tokens[tokens.size() - 1]);
					}
					else if (tokens[tokens.size() - 3] == "heightScale") {
						dimensions.heightScale = std::stof(tokens[tokens.size() - 1]);
					}
					else if (tokens[tokens.size() - 3] == "depthScale") {
						dimensions.depthScale = std::stof(tokens[tokens.size() - 1]);
					}
				}
			}
			tokens.clear();
		}
	}
	return true;
}

void VoxelData::initFBOs(unsigned int contextWidth, unsigned int contextHeight)
{
	// Enter FBO
	glGenFramebuffers(1, &enterFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, enterFBO);

	glGenTextures(1, &enterTexture);
	glBindTexture(GL_TEXTURE_2D, enterTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, contextWidth, contextHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, enterTexture, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, contextWidth, contextHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Exit FBO
	glGenFramebuffers(1, &exitFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, exitFBO);

	glGenTextures(1, &exitTexture);
	glBindTexture(GL_TEXTURE_2D, exitTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, contextWidth, contextHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, exitTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, contextWidth, contextHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

}

void VoxelData::initQuad()
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

	quadVAO.Bind();
	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	quadVAO.Unbind();
}

VoxelData::VoxelData(Shader* _shader, Shader* _boundingShader, const char* directory, unsigned int contextWidth, unsigned int contextHeight)
		: shader(_shader),
	maxIntensity(255),
	maxAttenuation(255),
	resolution(256, 256, 99),
	plane(glm::vec3(100,100,50), glm::vec3(0,0,1)),
	exposure(1.0f),
	gamma(0.4f),
	lightPosition(glm::vec3(256,100,256)),
	lightIntensity(glm::vec3(10500, 10500, 10000)),
	boundingGeometry(_boundingShader),
	scale(1.0f, 1.0f, 1.0f),
	position(0.0f, 256.0f, 0.0f),
	normal(0.0f, 0.0f, 1.0f),
	up(0.0f, 1.0f, 0.0f),
	shadowSamples(10)
	{
	// Stores the width, height, and the number of color channels of the image
	Dimensions dimensions;
	if (readDimensions(std::string(directory).append("dimensions.txt").c_str(), name, dimensions)) {
		voxels = new Texture3D(directory, dimensions, 0, GL_RED, GL_UNSIGNED_BYTE);
		scale = glm::vec3(dimensions.widthScale, dimensions.heightScale, dimensions.depthScale);
	}
	else {
		throw new std::exception("Failed to read dimensions of voxel data!");
	}

	unsigned char* transferBytes = defaultTransferFunction(256);
	transferFunction = new Texture1D(transferBytes, 256, 1, GL_RGBA, GL_UNSIGNED_BYTE);
	delete[] transferBytes;

	initQuad();
	initFBOs(contextWidth, contextHeight);
	boundingGeometry.updateGeometry(*voxels);

	updateModelMatrix();
}

VoxelData::~VoxelData() {
	quadVAO.Delete();
	voxels->Delete();
	delete voxels;
	transferFunction->Delete();
	delete transferFunction;
}

void VoxelData::animate(float dt)
{
	glm::mat4 M(1);
	M = glm::rotate(M, dt * 0.001f, glm::vec3(0, 1, 0));
	glm::vec4 rotated = M * glm::vec4(lightPosition, 1);
	lightPosition = rotated;

}

void VoxelData::optimize(float dt, bool paused, float cameraLastActive) {
	static int hardCap = 20;
	if (paused) {
		int c = cameraLastActive / 20;
		hardCap = 0.8 * hardCap + 0.2 * 30.0 / dt;
		shadowSamples = (hardCap >= c)? c : hardCap;
	}
	else {
		shadowSamples = 3;
	}
}


void VoxelData::draw(Camera& camera) {
	boundingGeometry.draw(camera, modelMatrix, enterFBO, exitFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
	quadVAO.Bind();
	shader->Activate();
	this->exportData();
	camera.exportData(*shader);
	voxels->Bind();
	transferFunction->Bind();

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, enterTexture);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, exitTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
	voxels->Unbind();
	transferFunction->Unbind();
}

void VoxelData::updateModelMatrix()
{
	modelMatrix = glm::translate(position) *  glm::orientation(normal, up) *  glm::scale(scale);
	invModelMatrix = glm::inverse(modelMatrix);
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
