#include "VoxelData.h"
#include <vector>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>


void VoxelData::exportData()
{
	Dimensions dim = voxels->getDimensions();
	glUniform3f(glGetUniformLocation(shader->ID, "resolution"),
		dim.width , dim.height, dim.depth);
	glUniform3f(glGetUniformLocation(shader->ID, "light1.position"),
		light1.position.x, light1.position.y, light1.position.z);
	glUniform3f(glGetUniformLocation(shader->ID, "light1.intensity"),
		light1.intensity.x, light1.intensity.y, light1.intensity.z);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "light1.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(light1.viewProjMatrix));
	glUniform1f(glGetUniformLocation(shader->ID, "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader->ID, "gamma"), gamma);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
	glUniform1ui(glGetUniformLocation(shader->ID, "shadowSamples"), shadowSamples);
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
	if (enterFBO > 0) {	// Not the first call
		glDeleteFramebuffers(1, &enterFBO);
		glDeleteFramebuffers(1, &exitFBO);
		glDeleteFramebuffers(1, &lightFBO);
	}

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

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);	// The same render buffer to both exit and enter FBOs.

	glGenFramebuffers(1, &lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);

	glGenTextures(1, &lightTexture);
	glBindTexture(GL_TEXTURE_2D, lightTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 2048, 2048, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightTexture, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 2048, 2048);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
}


VoxelData::VoxelData(Shader* _shader, Shader* _boundingShader, Shader* _transferShader, VAO* quadVAO, const char* directory, unsigned int contextWidth, unsigned int contextHeight)
		: shader(_shader),
	maxIntensity(255),
	maxAttenuation(255),
	plane(glm::vec3(100,100,50), glm::vec3(0,0,1)),
	exposure(1.1f),
	gamma(0.98f),
	boundingGeometry(_boundingShader),
	transferFunction(_transferShader, quadVAO),
	refereceSpatialTransferFunction(_transferShader, quadVAO),
	scale(1.0f, 1.0f, 1.0f),
	position(0.0f, 0.0f, 0.0f),
	normal(0.0f, 0.0f, 1.0f),
	up(0.0f, 1.0f, 0.0f),
	eulerAngles(0.0f, 0.0f, 0.0f),
	shadowSamples(3),
	quadVAO(quadVAO),
	boundingGeometryTreshold(0.006f),
	transferFloodFillTreshold(4.0f),
	STFradius(0.2f),
	STFEmission(1.0f),
	STFOpacity(1.0f)
	{
	// Stores the width, height, and the number of color channels of the image
	Dimensions dimensions;
	if (readDimensions(std::string(directory).append("dimensions.txt").c_str(), name, dimensions)) {
		voxels = new Texture3D(directory, dimensions, 0, GL_RED);
		scale = glm::vec3(dimensions.widthScale, dimensions.heightScale, dimensions.depthScale);
	}
	else {
		throw new std::exception("Failed to read dimensions of voxel data!");
	}
	
	// Transfer function:
	refereceSpatialTransferFunction.spatialTransferFunction(glm::ivec2(256, 128), *voxels, STFradius, STFOpacity, STFEmission);
	resetToDefault();
	transferFunction.setCamSpacePosition(glm::vec2(0.5f, -0.8f));
	refereceSpatialTransferFunction.setCamSpacePosition(glm::vec2(-0.5f, -0.8f));

	initFBOs(contextWidth, contextHeight);

	light1.position = glm::vec3(300, 50, 300);
	light1.intensity = glm::vec3(110000, 110000, 90000);
	
	updateMatrices();
}

VoxelData::~VoxelData() {
	voxels->Delete();
	delete voxels;
}

void VoxelData::animate(float dt)
{
//	glm::mat4 M = glm::rotate(dt * 0.001f, up);
//	normal = M * glm::vec4(normal, 0);
	eulerAngles.y += dt * 0.0001;
	updateMatrices();
}

void VoxelData::optimize(float dt, bool paused, float cameraLastActive) {
	return;
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
	boundingGeometry.draw(camera, light1, modelMatrix, invModelMatrix, enterFBO, exitFBO, lightFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	quadVAO->Bind();
	shader->Activate();
	this->exportData();
	camera.exportData(*shader);
	voxels->Bind();
	transferFunction.Bind();

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, enterTexture);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, exitTexture);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, lightTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
	voxels->Unbind();
	transferFunction.Unbind();
	transferFunction.draw();
	refereceSpatialTransferFunction.draw();
}


void VoxelData::updateMatrices()
{
	Dimensions dim = voxels->getDimensions();
	modelMatrix =
		glm::translate(position)
		* glm::rotate(eulerAngles.x, glm::vec3(1, 0, 0))
		* glm::rotate(eulerAngles.z, glm::vec3(0, 0, 1))
		* glm::rotate(eulerAngles.y, glm::vec3(0, 1, 0))
		* glm::orientation(normal, up)
		* glm::scale(scale)
		* glm::translate(glm::vec3(dim.width, dim.height, dim.depth) * -0.5f);	// Origo to center of volume.
	invModelMatrix = glm::inverse(modelMatrix);

	glm::mat4 view = glm::lookAt(light1.position, light1.position + glm::normalize(position - light1.position), up);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 1000.0f);
	light1.viewProjMatrix = projection * view;
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

void VoxelData::selectTransferFunctionRegion(double xCamPos, double yCamPos)
{
	glm::vec4 camPos = glm::vec4(xCamPos, yCamPos, 0, 1);
	glm::vec4 modelPos = transferFunction.getInvModelMatrix() * camPos;
	glm::vec2 texCoords = glm::vec2(modelPos.x / 2.0f + 0.5f, 0.5f + modelPos.y / 2.0f);
	bool inBound = false;
	if (texCoords.x >= 0.0f && texCoords.x <= 1.0f
		&& texCoords.y >= 0.0f && texCoords.y <= 1.0f) {
		inBound = true;
	}
	else {
		modelPos = refereceSpatialTransferFunction.getInvModelMatrix() * camPos;
		texCoords = glm::vec2(modelPos.x / 2.0f + 0.5f, 0.5f + modelPos.y / 2.0f);
		if (texCoords.x >= 0.0f && texCoords.x <= 1.0f
			&& texCoords.y >= 0.0f && texCoords.y <= 1.0f) {
			inBound = true;
		}
	}

	if (inBound) {
		if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[0])) {
			glm::vec3 color = refereceSpatialTransferFunction(texCoords);
			transferFunction = refereceSpatialTransferFunction;
			transferFunction.floodFill(texCoords, glm::vec4(color.r, color.g, color.b, 1), transferFloodFillTreshold);
			transferFunction.blur(3);
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[1])) {
			transferFunction.crop(texCoords - glm::vec2(0.2, 0.3), texCoords + glm::vec2(0.2, 0.3));
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[2])) {
			glm::vec3 color = refereceSpatialTransferFunction(texCoords);
			transferFunction = refereceSpatialTransferFunction;
			transferFunction.singleColor(color);
			transferFunction.blur(3);
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[3])) {
			glm::vec3 color = refereceSpatialTransferFunction(texCoords);
			transferFunction.removeColor(color);
		}
		boundingGeometry.updateGeometry(*voxels, transferFunction, boundingGeometryTreshold);
	}
}

void VoxelData::resetToSTF()
{
	refereceSpatialTransferFunction.spatialTransferFunction(glm::ivec2(256, 128), *voxels, STFradius, STFOpacity, STFEmission);
	transferFunction = refereceSpatialTransferFunction;
	boundingGeometry.updateGeometry(*voxels, transferFunction, boundingGeometryTreshold);
}

void VoxelData::resetToDefault()
{
	transferFunction.defaultTransferFunction(glm::ivec2(256, 128));
	boundingGeometry.updateGeometry(*voxels, transferFunction, boundingGeometryTreshold);
}
