#include "VoxelData.h"
#include "AssetManager.h"

#include <vector>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>


void VoxelData::exportData(Shader* shader)
{
	Dimensions dim = voxelTexture->getDimensions();
	glUniform3f(glGetUniformLocation(shader->ID, "resolution"),
		dim.width , dim.height, dim.depth);
	glUniform1f(glGetUniformLocation(shader->ID, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(shader->ID, "specularColor"), specularColor.r, specularColor.g, specularColor.b);
	glUniform3f(glGetUniformLocation(shader->ID, "ambientColor"), ambientColor.r, ambientColor.g, ambientColor.b);
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "sceneObject.invModelMatrix"), 1, GL_FALSE, glm::value_ptr(invModelMatrix));
	glUniform1ui(glGetUniformLocation(shader->ID, "shadowSamples"), shadowSamples);
	slicingPlane.exportData(shader, "slicingPlane");
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
		for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
			delete lightTextures[i];
		}
		delete opacityTextures[0];
		delete opacityTextures[1];
		delete quadTexture;
		delete quadDepthTexture;
	}

	RBO enterExitRbo(GL_DEPTH_COMPONENT24, contextWidth, contextHeight);

	// Enter:
	enterTexture = new Texture2D(GL_RGBA16F, glm::vec2(contextWidth, contextHeight), 2, GL_RGBA, GL_UNSIGNED_BYTE);
	enterFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *enterTexture, 0);
	enterFBO.LinkRBO(GL_DEPTH_ATTACHMENT, enterExitRbo);

	// Exit:
	exitTexture = new Texture2D(GL_RGBA16F, glm::vec2(contextWidth, contextHeight), 3, GL_RGBA, GL_UNSIGNED_BYTE);
	exitFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *exitTexture, 0);
	exitFBO.LinkRBO(GL_DEPTH_ATTACHMENT, enterExitRbo);

	// Opacity:
	opacityTextures[0] = new Texture2D(GL_RGBA, glm::vec2(contextWidth, contextHeight), 4, GL_RGBA, GL_FLOAT);
	opacityTextures[1] = new Texture2D(GL_RGBA, glm::vec2(contextWidth, contextHeight), 4, GL_RGBA, GL_FLOAT);

	// SkyBox is at location 5.

	// Lights:
	for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
		lightTextures[i] = new Texture2D(GL_RGBA16F, glm::vec2(contextWidth, contextHeight), 7 + i, GL_RGBA, GL_UNSIGNED_BYTE);
		lightFBOs[i].LinkTexture(GL_COLOR_ATTACHMENT0, *lightTextures[i], 0);

		RBO lightRbo(GL_DEPTH_COMPONENT24, contextWidth, contextHeight);
		lightFBOs[i].LinkRBO(GL_DEPTH_ATTACHMENT, lightRbo);
	}

	// Quad:
	quadTexture = new Texture2D(GL_RGBA, glm::vec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
	voxelQuadFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *quadTexture, 0);
	quadDepthTexture = new Texture2D(GL_DEPTH_COMPONENT, glm::vec2(contextWidth, contextHeight), 1, GL_DEPTH_COMPONENT, GL_FLOAT);
	voxelQuadFBO.LinkTexture(GL_DEPTH_ATTACHMENT, *quadDepthTexture, 0);
}

VoxelData::VoxelData(Shader* _voxelShader, Shader* _voxelHalfAngle, Shader* quadShader, Shader* _boundingShader, Shader* _flatColorBoundingShader, Shader* _transferShader, VAO* quadVAO, const char* directory, unsigned int contextWidth, unsigned int contextHeight)
	: voxelShader(_voxelShader),
	voxelHalfAngleShader(_voxelHalfAngle),
	quadShader(quadShader),
	maxIntensity(255),
	maxAttenuation(255),
	exposure(1.1f),
	gamma(0.98f),
	boundingGeometry(_boundingShader, _flatColorBoundingShader),
	transferFunction(_transferShader, quadVAO),
	scale(1.0f, 1.0f, 1.0f),
	position(0.0f, 256 / 2.0f, 0.0f),
	normal(0.0f, 0.0f, 1.0f),
	up(0.0f, 1.0f, 0.0f),
	animationEulerAngles(0.0f, 0.0f, 0.0f),
	staticEulerAngles(0.0f, 0.0f, 0.0f),
	shadowSamples(10),
	quadVAO(quadVAO),
	boundingGeometryTreshold(0.000000000001f),
	transferFloodFillTreshold(4.0f),
	STFradius(0.25f),
	STFEmission(1.0f),
	STFOpacity(1.0f),
	shininess(10.0f),
	specularColor(0.56f, 0.56f, 0.5f),
	ambientColor(0.005f, 0.005f, 0.005f),
	slicingPlane(glm::vec3(position), glm::vec3(1, 0, 0)) {

	featureGroups.clear();
	// Stores the width, height, and the number of color channels of the image
	Dimensions dimensions;
	if (!readDimensions(std::string(directory).append("dimensions.txt").c_str(), name, dimensions)) {
		throw new std::exception("Failed to read dimensions of voxel data!");
	}
	voxelTexture = new Texture3D(directory, dimensions, 0, GL_RED);
	scale = glm::vec3(dimensions.widthScale, dimensions.heightScale, dimensions.depthScale);
	initBoundingBox(dimensions, boundingBox);

	// Transfer function:
	transferFunction.setCamSpacePosition(glm::vec2(0.0f, -0.8f));

	initFBOs(contextWidth, contextHeight);

	updateMatrices();
}

VoxelData::~VoxelData() {
	delete voxelTexture;
	if (enterTexture != nullptr) {
		delete enterTexture;
	}
	if (exitTexture != nullptr) {
		delete exitTexture;
	}
	for (int i = 0; i < MAX_LIGHT_COUNT; i++) {
		if (lightTextures[i] != nullptr) {
			delete lightTextures[i];
		}
	}
	enterFBO.Delete();
	exitFBO.Delete();
	for (int i = 0; i < MAX_LIGHT_COUNT; i++) {

	}

}

void VoxelData::animate(float dt)
{
	transferFunction.animate(dt);
	if (animation != nullptr) {
		animationEulerAngles.y += dt * 0.0001;
		updateMatrices();
		changed = true;
	}
}

void VoxelData::control(float dt, bool paused, float cameraLastActive) {
}

void VoxelData::drawBoundingGeometry(Camera& camera, std::vector<Light>& lights) {
	for (int i = 0; i < lights.size(); i++) {
		glm::vec3 lightPos(lights[i].position.x, lights[i].position.y, lights[i].position.z);
		float w = lights[i].position.w;
		glm::vec3 eye(lightPos + lightPos * (300.0f - 300.0f * w));
		glm::mat4 view = glm::lookAt(eye, eye + glm::normalize(position - eye), up);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 1000.0f);
		lights[i].viewProjMatrix = projection * view;
	}
	boundingGeometry.draw(camera, lights, modelMatrix, invModelMatrix, enterFBO, exitFBO, lightFBOs);
}

void VoxelData::resetOpacity(Light& light)
{
	voxelQuadFBO.LinkTexture(GL_COLOR_ATTACHMENT1, *opacityTextures[0], 0);
	glClearDepth(1.0f);
	glClearColor(1 - light.powerDensity.r, 1 - light.powerDensity.g, 1 - light.powerDensity.b, 0);
	voxelQuadFBO.SelectDrawBuffers({ GL_COLOR_ATTACHMENT1 });
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	voxelQuadFBO.LinkTexture(GL_COLOR_ATTACHMENT1, *opacityTextures[1], 0);
	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 0);
	voxelQuadFBO.SelectDrawBuffers({ GL_COLOR_ATTACHMENT1 });
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearDepth(1.0f);
	glClearColor(0, 0, 0, 0);
	voxelQuadFBO.SelectDrawBuffers({ GL_COLOR_ATTACHMENT0 });
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	voxelQuadFBO.Unbind();
}

void VoxelData::drawBoundingGeometryOnScreen(FBO& fbo, Camera& camera, float opacity)
{
	boundingGeometry.drawOnScreen(fbo, camera, modelMatrix, invModelMatrix, opacity);
}

void VoxelData::drawTransferFunction(FBO& fbo) {
	transferFunction.draw(fbo);
}


void VoxelData::drawLayer(Camera& camera, Texture2D& targetDepthTeture, Light& light, SkyBox& skybox, unsigned int currentStep, unsigned int stepCount)
{
	unsigned int source = currentStep % 2;
	unsigned int target = (currentStep + 1) % 2;
	float depth = (currentStep + 1.0f) / (float)stepCount;
	voxelQuadFBO.LinkTexture(GL_COLOR_ATTACHMENT1, *opacityTextures[target], 0);

	voxelQuadFBO.Bind();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunci(0, GL_ONE, GL_ONE);
	glBlendFunci(1, GL_ONE, GL_ZERO);
	
	voxelQuadFBO.SelectDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
	quadVAO->Bind();
	voxelShader->Activate();
	this->exportData(voxelShader);
	camera.exportData(*voxelShader);
	light.exportData(*voxelShader, 0);

	glUniform1f(glGetUniformLocation(voxelShader->ID, "depth"), depth);
	glUniform1ui(glGetUniformLocation(voxelShader->ID, "stepCount"), stepCount);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTexture().ID);

	voxelTexture->Bind();
	transferFunction.Bind();
	enterTexture->Bind();
	exitTexture->Bind();
	targetDepthTeture.Bind();
	opacityTextures[source]->Bind();
	lightTextures[0]->Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	voxelTexture->Unbind();
	transferFunction.Unbind();
	enterTexture->Unbind();
	exitTexture->Unbind();
	opacityTextures[source]->Unbind();
}

void VoxelData::drawHalfAngleLayer(Camera& camera, Texture2D& targetDepthTeture, Light& light, SkyBox& skybox, unsigned int currentStep, unsigned int stepCount)
{
	float assumedDiameter = glm::length(scale * glm::vec3(voxelTexture->dimensions.width, voxelTexture->dimensions.height, voxelTexture->dimensions.depth));
	float delta = assumedDiameter / stepCount;

	glm::vec3 viewDir = glm::normalize(camera.eye - position);
	glm::vec3 lightDir = glm::normalize(glm::vec3(light.position.x, light.position.y, light.position.z) - position);
	glm::vec3 halfway;
	if (glm::dot(viewDir, lightDir) < 0.0) {
		halfway = glm::normalize(-viewDir + lightDir);
	}
	else {
		halfway = glm::normalize(viewDir + lightDir);
	}
	delta *= abs(glm::dot(halfway, viewDir));
	voxelQuadFBO.Bind();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	if (glm::dot(viewDir, lightDir) < 0.0) {
		glBlendFunci(0, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	// Back to front
	}
	else {
		glBlendFunci(0, GL_ONE_MINUS_DST_ALPHA, GL_ONE);	// Front to back
	}
	glBlendFunci(1, GL_ONE, GL_ZERO);

	voxelQuadFBO.LinkTexture(GL_DEPTH_ATTACHMENT, targetDepthTeture, 0);
	voxelQuadFBO.SelectDrawBuffers({ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 });
	voxelHalfAngleShader->Activate();
	this->exportData(voxelHalfAngleShader);
	camera.exportData(*voxelHalfAngleShader);
	light.exportData(*voxelHalfAngleShader, 0);

	glUniform1f(glGetUniformLocation(voxelHalfAngleShader->ID, "delta"), delta);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTexture().ID);

	voxelTexture->Bind();
	transferFunction.Bind();
	enterTexture->Bind();
	exitTexture->Bind();

	glm::vec4 modelSliceNormal = glm::vec4(halfway.x, halfway.y, halfway.z, 0.0) * modelMatrix;
	glUniform3f(glGetUniformLocation(voxelHalfAngleShader->ID, "modelSliceNormal"), modelSliceNormal.x, modelSliceNormal.y, modelSliceNormal.z);

	unsigned int source;
	unsigned int target;
	{		// Loop here
		source = currentStep % 2;
		target = (currentStep + 1) % 2;
		glm::vec3 slicePosition = position - halfway * abs(glm::dot(halfway, viewDir)) * assumedDiameter * (currentStep / (float)stepCount - 0.5f);
		glm::vec4 modelSlicePosition = invModelMatrix * glm::vec4(slicePosition.x, slicePosition.y, slicePosition.z, 1.0);
		modelSlicePosition = modelSlicePosition / modelSlicePosition.w;
		voxelQuadFBO.LinkTexture(GL_COLOR_ATTACHMENT1, *opacityTextures[target], 0);
		opacityTextures[source]->Bind();
		drawProxyGeometry(camera, modelSlicePosition, modelSliceNormal);
	}

	voxelTexture->Unbind();
	transferFunction.Unbind();
	enterTexture->Unbind();
	exitTexture->Unbind();
	opacityTextures[source]->Unbind();
	glDepthMask(GL_TRUE);
	voxelQuadFBO.Unbind();
}

void VoxelData::drawQuad(FBO& fbo) {
	fbo.Bind();
	quadShader->Activate();
	quadVAO->Bind();
	quadTexture->Bind();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	fbo.Unbind();
}


void VoxelData::updateMatrices()
{
	if (changed) {
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
}

void VoxelData::shiftIntersectionPlane(float delta)
{
	slicingPlane.setPoint(slicingPlane.getPoint() + delta * slicingPlane.getNormal());
	changed = true;
}

void VoxelData::rotateIntersectionPlane(float rad)
{
	glm::mat4 M(1);
	M = glm::rotate(M, rad, glm::vec3(0, 1, 0));
	glm::vec4 rotated = M * glm::vec4(slicingPlane.getNormal(), 1);
	slicingPlane.setNormal(rotated);
	changed = true;
}

void VoxelData::selectTransferFunctionRegion(double xCamPos, double yCamPos)
{
	if (!transferFunction.isVisible()) {
		return;
	}
	glm::vec4 camPos = glm::vec4(xCamPos, yCamPos, 0, 1);
	glm::vec4 modelPos = transferFunction.getInvModelMatrix() * camPos;
	glm::vec2 texCoords = glm::vec2(modelPos.x / 2.0f + 0.5f, 0.5f + modelPos.y / 2.0f);
	bool inBound = false;
	if (texCoords.x >= 0.0f && texCoords.x <= 1.0f
		&& texCoords.y >= 0.0f && texCoords.y <= 1.0f) {
		inBound = true;
	}

	if (inBound) {
		if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[0])) {
			glm::vec3 color = transferFunction(texCoords);
			transferFunction.floodFill(texCoords, glm::vec4(color.r, color.g, color.b, 1), transferFloodFillTreshold);
			transferFunction.blur(3);
			boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
			changed = true;
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[1])) {
			transferFunction.clear();
			transferFunction.crop(texCoords - glm::vec2(0.2, 0.3), texCoords + glm::vec2(0.2, 0.3));
			boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
			changed = true;
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[2])) {
			Feature* feature = transferFunction.getFeatureFromPosition(texCoords);
			if (nullptr != feature) {
				std::cout << "Selected: " << feature->name << std::endl;
				std::cout << "Feature element count: " << feature->elements.size() << std::endl;
				transferFunction.clear();
				transferFunction.setFeatureVisibility(*feature, true);
				transferFunction.blur(3);
				if (selectedFeature != feature) {
					selectedFeature = feature;
					boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
					changed = true;
				}
			}
		}
		else if (std::string(currentTransferRegionSelectMode) == std::string(transferRegionSelectModes[3])) {
			Feature* feature = transferFunction.getFeatureFromPosition(texCoords);
			if (nullptr != feature) {
				std::cout << "Removed: " << feature->name << std::endl;
				std::cout << "Feature element count: " << feature->elements.size() << std::endl;
				bool update = transferFunction.setFeatureVisibility(*feature, false);
				transferFunction.blur(3);
				if (update) {
					boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
					changed = true;
				}
			}
		}
	}
}

void VoxelData::resetToSTF()
{
	transferFunction.spatialTransferFunction(glm::ivec2(256, 128), *voxelTexture, STFradius, STFOpacity, STFEmission);
	std::cout << "STF feature count: " << transferFunction.getFeatures().size() << std::endl;
	boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
	changed = true;
}

void VoxelData::resetToDefault()
{
	transferFunction.clear();
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

void VoxelData::setSelectedFeature(const char* name) {
	selectedFeature = transferFunction.findFeatureByName(name);
	transferFunction.clear();
	if (selectedFeature != nullptr) {
		if (transferFunction.setFeatureVisibility(*selectedFeature, true)) {
			transferFunction.blur(3);
			update();
		};
	}
}

void VoxelData::update()
{
	boundingGeometry.updateGeometry(*voxelTexture, transferFunction, boundingGeometryTreshold);
	changed = true;
}

void VoxelData::redrawSelected()
{
	transferFunction.clear();
	transferFunction.setFeatureVisibility(*selectedFeature, true);
	transferFunction.blur(3);
	update();
}

static int nextGroupIdx = 1;
void VoxelData::createFeatureGroup()
{
	FeatureGroup group;
	group.features.clear();
	group.name = std::string("Group").append(std::to_string(nextGroupIdx++));
	featureGroups.push_back(group);
}

void VoxelData::addFeatureToFeatureGroup(Feature* feature) {
	if (nullptr != feature && nullptr != selectedFeatureGroup) {
		if (std::find(selectedFeatureGroup->features.begin(), selectedFeatureGroup->features.end(), feature)
			== selectedFeatureGroup->features.end()) {
			selectedFeatureGroup->features.push_back(feature);
		}
	}
}


void VoxelData::addSelectedFeatureToFeatureGroup() 
{
	if (nullptr != selectedFeature && nullptr != selectedFeatureGroup) {
		if (std::find(selectedFeatureGroup->features.begin(), selectedFeatureGroup->features.end(), selectedFeature) 
			== selectedFeatureGroup->features.end()) {
				selectedFeatureGroup->features.push_back(selectedFeature);
		}
	}
}

void VoxelData::removeSelectedFeatureFromFeatureGroup()
{
	if (nullptr != selectedFeature && nullptr != selectedFeatureGroup) {
		selectedFeatureGroup->features.erase(std::find(selectedFeatureGroup->features.begin(),
			selectedFeatureGroup->features.end(), 
			selectedFeature));
		selectedFeature = nullptr;
	}
}

void VoxelData::showSelectedFeatureGroup()
{
	if (nullptr != selectedFeatureGroup) {
		transferFunction.clear();
		for (Feature* feature : selectedFeatureGroup->features) {
			feature->visible = true;
		}
		transferFunction.showVisible();
		transferFunction.blur(3);
		update();
	}
}

void VoxelData::saveFeatures() {
	std::ofstream stream(AssetManager::getInstance()->getSavesFolderPath().append("/features.txt"));
	if (stream.is_open()) {
		transferFunction.saveFeatures(stream);
		for (FeatureGroup& group : featureGroups) {
			group.save(stream);
		}
		stream.close();
	}
}

TransferFunction* VoxelData::getTransferFunction() {
	return &transferFunction;
}

Feature* VoxelData::getSelectedFeature() {
	return selectedFeature;
}

void VoxelData::setAnimation(Animation* animation) {
	this->animation = animation;
}

void VoxelData::cycleSelectedFeature() {
	Feature* prevSelected = selectedFeature;
	selectedFeature = transferFunction.nextFeature(selectedFeature);
	if (prevSelected != selectedFeature) {
		std::cout << "Next: " << selectedFeature->name << std::endl;
		transferFunction.clear();
		transferFunction.setFeatureVisibility(*selectedFeature, true);
		transferFunction.blur(3);
		update();
	}
}

void VoxelData::showAll() {
	transferFunction.showAll();
	update();
}


void VoxelData::loadFeatures() {
	std::ifstream stream(AssetManager::getInstance()->getSavesFolderPath().append("/features.txt"));
	if (stream.is_open()) {
		featureGroups.clear();
		transferFunction.loadFeatures(stream, featureGroups);
		stream.close();
		FeatureGroup all;
		all.name = "All features";
		for (Feature& feature : transferFunction.getFeatures()) {
			all.features.push_back(&feature);
		}
		all.serialize = false;
		featureGroups.push_back(all);
		selectedFeature = nullptr;
		selectedFeatureGroup = nullptr;
		update();
	}
}

void VoxelData::drawProxyGeometry(const Camera& camera, const glm::vec3& modelSlicePosition, const glm::vec3& modelSliceNormal) {

	glm::vec3 modelIntersection;
	std::vector<glm::vec3> vertices;
	vertices.clear();
	for (int i = 0; i < 12; i++) {
		if (intersectPlane(boundingBox.edges[i], modelSlicePosition, modelSliceNormal, modelIntersection)) {
			vertices.push_back(modelIntersection);
		}
	}
	if (vertices.size() >= 3) {
		std::vector<GLuint> indices;
		indices.clear();
		
		
		if (vertices.size() == 3) {	// Single triangle
			indices.push_back(0);
			indices.push_back(1);
			indices.push_back(2);
		}
		else {	// Triangle fan
			glm::vec3 center = glm::vec3(0.0);
			for (int i = 0; i < vertices.size(); i++) {
				center += vertices[i];
			}
			center /= (float)vertices.size();
			glm::vec4 cameraCenter = camera.viewProjMatrix * modelMatrix * glm::vec4(center.x, center.y, center.z, 1.0f);
			cameraCenter /= cameraCenter.w;
			std::vector<glm::vec3> orderedVertices;
			const glm::mat4& modelMatrix = this->modelMatrix;
			auto compareAngle = [camera, center, modelMatrix, cameraCenter](glm::vec3& vertex1 , glm::vec3& vertex2)-> float {
				glm::vec4 cameraVertex = camera.viewProjMatrix * modelMatrix * glm::vec4(vertex1.x, vertex1.y, vertex1.z, 1.0f);
				cameraVertex /= cameraVertex.w;
				glm::vec2 dir1 = glm::normalize(glm::vec2(cameraVertex.x, cameraVertex.y) - glm::vec2(cameraCenter.x, cameraCenter.y));
				
				cameraVertex = camera.viewProjMatrix * modelMatrix * glm::vec4(vertex2.x, vertex2.y, vertex2.z, 1.0f);
				cameraVertex /= cameraVertex.w;
				glm::vec2 dir2 = glm::normalize(glm::vec2(cameraVertex.x, cameraVertex.y) - glm::vec2(cameraCenter.x, cameraCenter.y));
				return atan2f(dir1.x, dir1.y) > atan2f(dir2.x, dir2.y);
			};
			std::sort(vertices.begin(), vertices.end(), compareAngle);
			
			for (int i = 0; i < vertices.size() - 2; i++) {
				indices.push_back(0);
				indices.push_back(i + 1);
				indices.push_back(i + 2);
			}
		}
		
		VAO VAO;
		VAO.Bind();
		VBO VBO(vertices);
		EBO EBO(indices);
		VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), (void*)0);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		VBO.Delete();
		EBO.Delete();
		VAO.Delete();
		VAO.Unbind();
		VAO.Delete();
	}
}

void VoxelData::initBoundingBox(Dimensions& dim, BoundingBox& box) {
	glm::vec3 resolution = glm::vec3(dim.width, dim.height, dim.depth);
	
	// Base quad:
	box.edges[0].position = glm::vec3(0,0,0) * resolution;
	box.edges[0].direction = glm::vec3(1,0,0);
	box.edges[0].length = resolution.x;

	box.edges[1].position = glm::vec3(1,0,0) * resolution;
	box.edges[1].direction = glm::vec3(0, 0, 1);
	box.edges[1].length = resolution.z;

	box.edges[2].position = glm::vec3(1, 0, 1) * resolution;
	box.edges[2].direction = glm::vec3(-1, 0, 0);
	box.edges[2].length = resolution.x;

	box.edges[3].position = glm::vec3(0, 0, 1) * resolution;
	box.edges[3].direction = glm::vec3(0, 0, -1);
	box.edges[3].length = resolution.z;

	// Vertical edges:
	box.edges[4].position = glm::vec3(0) * resolution;
	box.edges[4].direction = glm::vec3(0, 1, 0);
	box.edges[4].length = resolution.y;

	box.edges[5].position = glm::vec3(1, 0, 0) * resolution;
	box.edges[5].direction = glm::vec3(0, 1, 0);
	box.edges[5].length = resolution.y;

	box.edges[6].position = glm::vec3(1, 0, 1) * resolution;
	box.edges[6].direction = glm::vec3(0, 1, 0);
	box.edges[6].length = resolution.y;

	box.edges[7].position = glm::vec3(0, 0, 1) * resolution;
	box.edges[7].direction = glm::vec3(0, 1, 0);
	box.edges[7].length = resolution.y;

	// Top quad:
	box.edges[8].position = glm::vec3(0, 1, 0) * resolution;
	box.edges[8].direction = glm::vec3(1, 0, 0);
	box.edges[8].length = resolution.x;

	box.edges[9].position = glm::vec3(1, 1, 0) * resolution;
	box.edges[9].direction = glm::vec3(0, 0, 1);
	box.edges[9].length = resolution.z;

	box.edges[10].position = glm::vec3(1, 1, 1) * resolution;
	box.edges[10].direction = glm::vec3(-1, 0, 0);
	box.edges[10].length = resolution.x;

	box.edges[11].position = glm::vec3(0, 1, 1) * resolution;
	box.edges[11].direction = glm::vec3(0, 0, -1);
	box.edges[11].length = resolution.z;
}

