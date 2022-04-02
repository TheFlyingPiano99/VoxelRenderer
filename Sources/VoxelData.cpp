#include "VoxelData.h"
#include <vector>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>


void VoxelData::exportData()
{
	Dimensions dim = voxelTexture->getDimensions();
	glUniform3f(glGetUniformLocation(shader->ID, "resolution"),
		dim.width , dim.height, dim.depth);
	glUniform1f(glGetUniformLocation(shader->ID, "exposure"), exposure);
	glUniform1f(glGetUniformLocation(shader->ID, "gamma"), gamma);
	glUniform1f(glGetUniformLocation(shader->ID, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(shader->ID, "specularColor"), specularColor.r, specularColor.g, specularColor.b);
	glUniform3f(glGetUniformLocation(shader->ID, "ambientColor"), ambientColor.r, ambientColor.g, ambientColor.b);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
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
	if (enterTexture != nullptr) {	// Not the first call
		delete enterTexture;
		delete exitTexture;
		for (int i = 0; i < 16; i++) {
			delete lightTextures[i];
		}
	}

	RBO enterExitRbo(GL_DEPTH_COMPONENT24, contextWidth, contextHeight);

	enterTexture = new Texture2D(GL_RGBA16F, glm::vec2(contextWidth, contextHeight), 2, GL_RGBA, GL_UNSIGNED_BYTE);
	enterFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *enterTexture, 0);
	enterFBO.LinkRBO(GL_DEPTH_ATTACHMENT, enterExitRbo);

	exitTexture = new Texture2D(GL_RGBA16F, glm::vec2(contextWidth, contextHeight), 3, GL_RGBA, GL_UNSIGNED_BYTE);
	exitFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *exitTexture, 0);
	exitFBO.LinkRBO(GL_DEPTH_ATTACHMENT, enterExitRbo);

	for (int i = 0; i < 16; i++) {
		lightTextures[i] = new Texture2D(GL_RGBA16F, glm::vec2(contextWidth, contextHeight), 4 + i, GL_RGBA, GL_UNSIGNED_BYTE);
		lightFBOs[i].LinkTexture(GL_COLOR_ATTACHMENT0, *lightTextures[i], 0);

		RBO lightRbo(GL_DEPTH_COMPONENT24, contextWidth, contextHeight);
		lightFBOs[i].LinkRBO(GL_DEPTH_ATTACHMENT, lightRbo);
	}
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
	referenceSpatialTransferFunction(_transferShader, quadVAO),
	scale(1.0f, 1.0f, 1.0f),
	position(0.0f, 300.0f, 0.0f),
	normal(0.0f, 0.0f, 1.0f),
	up(0.0f, 1.0f, 0.0f),
	animationEulerAngles(0.0f, 0.0f, 0.0f),
	staticEulerAngles(0.0f, 0.0f, 0.0f),
	shadowSamples(2),
	quadVAO(quadVAO),
	boundingGeometryTreshold(0.006f),
	transferFloodFillTreshold(4.0f),
	STFradius(0.2f),
	STFEmission(1.0f),
	STFOpacity(1.0f),
	shininess(20.0f),
	specularColor(0.2f, 0.2f, 0.2f),
	ambientColor(0.0001f, 0.0001f, 0.0001f)
	{
	// Stores the width, height, and the number of color channels of the image
	Dimensions dimensions;
	if (readDimensions(std::string(directory).append("dimensions.txt").c_str(), name, dimensions)) {
		voxelTexture = new Texture3D(directory, dimensions, 0, GL_RED);
		scale = glm::vec3(dimensions.widthScale, dimensions.heightScale, dimensions.depthScale);
	}
	else {
		throw new std::exception("Failed to read dimensions of voxel data!");
	}
	
	// Transfer function:
	referenceSpatialTransferFunction.spatialTransferFunction(glm::ivec2(256, 128), *voxelTexture, STFradius, STFOpacity, STFEmission);
	resetToDefault();
	transferFunction.setCamSpacePosition(glm::vec2(0.5f, -0.8f));
	referenceSpatialTransferFunction.setCamSpacePosition(glm::vec2(-0.5f, -0.8f));

	initFBOs(contextWidth, contextHeight);

	updateMatrices();
}

VoxelData::~VoxelData() {
	voxelTexture->Delete();
	delete voxelTexture;

}

void VoxelData::animate(float dt)
{
//	glm::mat4 M = glm::rotate(dt * 0.001f, up);
//	normal = M * glm::vec4(normal, 0);
	animationEulerAngles.y += dt * 0.0001;
	updateMatrices();
	changed = true;
}

void VoxelData::control(float dt, bool paused, float cameraLastActive) {
}


void VoxelData::draw(Camera& camera, std::vector<Light>& lights, FBO& quadFBO, glm::vec2 scale, glm::vec2 offset, float depthLimit) {
	for (int i = 0; i < lights.size(); i++) {
		glm::vec3 lightPos(lights[i].position.x, lights[i].position.y, lights[i].position.z);
		float w = lights[i].position.w;
		glm::vec3 eye(lightPos + lightPos * (300.0f - 300.0f * w));
		glm::mat4 view = glm::lookAt(eye, eye + glm::normalize(position - eye), up);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 1000.0f);
		lights[i].viewProjMatrix = projection * view;
	}
	boundingGeometry.draw(camera, lights, modelMatrix, invModelMatrix, enterFBO, exitFBO, lightFBOs);

	quadFBO.Bind();
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	quadVAO->Bind();
	shader->Activate();
	this->exportData();
	camera.exportData(*shader);
	for (int i = 0; i < lights.size(); i++) {
		lights[i].exportData(*shader, i);
	}
	glUniform1ui(glGetUniformLocation(shader->ID, "lightCount"), lights.size());
	glUniform1f(glGetUniformLocation(shader->ID, "depthLimit"), depthLimit);


	voxelTexture->Bind();
	transferFunction.Bind();
	enterTexture->Bind();
	exitTexture->Bind();

	for (int i = 0; i < lights.size(); i++) {
		lightTextures[i]->Bind();
	}

	/*
	glUniform1i(glGetUniformLocation(shader->ID, "voxels"), voxelTexture->unit);
	glUniform1i(glGetUniformLocation(shader->ID, "colorAttenuationTransfer"), 1);
	glUniform1i(glGetUniformLocation(shader->ID, "enterTexture"), 2);
	glUniform1i(glGetUniformLocation(shader->ID, "exitTexture"), 3);
	*/
	
	/*
	for (int i = 0; i < lights.size(); i++) {	// Setting unirom binding of light textures
		std::string uniformName("lights[");
		uniformName.append(std::to_string(i)).append("].exitTexture");
		glUniform1i(glGetUniformLocation(shader->ID, uniformName.c_str()), 4 + i);
	}
	*/
	glUniform2f(glGetUniformLocation(shader->ID, "scale"), scale.x, scale.y);
	glUniform2f(glGetUniformLocation(shader->ID, "offset"), offset.x, offset.y);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glEnable(GL_CULL_FACE);
	voxelTexture->Unbind();
	transferFunction.Unbind();
	enterTexture->Unbind();
	exitTexture->Unbind();

	transferFunction.draw();
	referenceSpatialTransferFunction.draw();
}


void VoxelData::updateMatrices()
{
	Dimensions dim = voxelTexture->getDimensions();
	modelMatrix =
		glm::translate(position)
		* glm::rotate(animationEulerAngles.x, glm::vec3(1, 0, 0))
		* glm::rotate(animationEulerAngles.z, glm::vec3(0, 0, 1))
		* glm::rotate(animationEulerAngles.y, glm::vec3(0, 1, 0))
		* glm::rotate(staticEulerAngles.x, glm::vec3(1, 0, 0))
		* glm::rotate(staticEulerAngles.z, glm::vec3(0, 0, 1))
		* glm::rotate(staticEulerAngles.y, glm::vec3(0, 1, 0))
		* glm::orientation(normal, up)
		* glm::scale(scale)
		* glm::translate(glm::vec3(dim.width, dim.height, dim.depth) * -0.5f);	// Origo to center of volume.
	invModelMatrix = glm::inverse(modelMatrix);
}

void VoxelData::shiftIntersectionPlane(float delta)
{
	plane.setPoint(plane.getPoint() + delta * plane.getNormal());
	changed = true;
}

void VoxelData::rotateIntersectionPlane(float rad)
{
	glm::mat4 M(1);
	M = glm::rotate(M, rad, glm::vec3(0, 1, 0));
	glm::vec4 rotated = M * glm::vec4(plane.getNormal(), 1);
	plane.setNormal(rotated);
	changed = true;
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
		modelPos = referenceSpatialTransferFunction.getInvModelMatrix() * camPos;
		texCoords = glm::vec2(modelPos.x / 2.0f + 0.5f, 0.5f + modelPos.y / 2.0f);
		if (texCoords.x >= 0.0f && texCoords.x <= 1.0f
			&& texCoords.y >= 0.0f && texCoords.y <= 1.0f) {
			inBound = true;
		}
	}

	if (inBound) {
		if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[0])) {
			glm::vec3 color = referenceSpatialTransferFunction(texCoords);
			transferFunction = referenceSpatialTransferFunction;
			transferFunction.floodFill(texCoords, glm::vec4(color.r, color.g, color.b, 1), transferFloodFillTreshold);
			transferFunction.blur(3);
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[1])) {
			transferFunction.crop(texCoords - glm::vec2(0.2, 0.3), texCoords + glm::vec2(0.2, 0.3));
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[2])) {
			glm::vec3 color = referenceSpatialTransferFunction(texCoords);
			transferFunction = referenceSpatialTransferFunction;
			transferFunction.singleColor(color);
			transferFunction.blur(3);
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[3])) {
			glm::vec3 color = referenceSpatialTransferFunction(texCoords);
			transferFunction.removeColor(color);
		}
		boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
	}
	changed = true;
}

void VoxelData::resetToSTF()
{
	referenceSpatialTransferFunction.spatialTransferFunction(glm::ivec2(256, 128), *voxelTexture, STFradius, STFOpacity, STFEmission);
	transferFunction = referenceSpatialTransferFunction;
	boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
	changed = true;
}

void VoxelData::resetToDefault()
{
	transferFunction.defaultTransferFunction(glm::ivec2(256, 128));
	boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
	changed = true;
}

void VoxelData::mergeVisibleClasses() {
	transferFunction.grayscale();
	transferFunction.blur(3);
	changed = true;
}

void VoxelData::rotateModelAroundX(float rad) {
	staticEulerAngles.x += rad;
	changed = true;
}

void VoxelData::rotateModelAroundY(float rad) {
	staticEulerAngles.y += rad;
	changed = true;
}

void VoxelData::rotateModelAroundZ(float rad) {
	staticEulerAngles.z += rad;
	changed = true;
}

bool VoxelData::popChanged() {
	bool prevChanged = changed;
	changed = false;
	return prevChanged;
}
