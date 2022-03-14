#include "TransferFunction.h"

#include<glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/rotate_vector.hpp>


TransferFunction::TransferFunction(Shader* shader, VAO* quad) : shader(shader), quadVAO(quad) {
	defaultTransferFunction(glm::ivec2(256, 64));
	setCamSpacePosition(glm::vec2(0.0f, -0.75f));
}

void TransferFunction::crop(glm::vec2 min, glm::vec2 max)
{
	glm::ivec2 dim = getDimensions();
	glm::ivec2 iMin = glm::ivec2(dim.x * min.x, dim.y * min.y);
	glm::ivec2 iMax = glm::ivec2(dim.x * max.x, dim.y * max.y);
	glm::vec2 center = (min + max) / 2.0f;
	glm::ivec2 iCenter = glm::ivec2(dim.x * center.x, dim.y * center.y);
	std::vector<glm::vec4> bytes(dim.x * dim.y);
	float radius = (iMax.x - iMin.x) / 2.0f;
	for (int y = 0; y < dim.y; y++) {
		for (int x = 0; x < dim.x; x++) {
			if (x >= iMin.x && x <= iMax.x && y >= iMin.y && y <= iMax.y) {
				bytes[y * dim.x + x] = glm::vec4(1.0f) * (1.0f - glm::length(glm::vec2(iCenter) - glm::vec2(x, y)) / radius);
			}
			else {
				bytes[y * dim.x + x] = glm::vec4(0.0f);
			}
		}
	}
	if (texture != nullptr) {
		delete texture;
	}
	texture = new Texture2D(bytes, dim, 1, GL_RGBA, GL_FLOAT);
}

void TransferFunction::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	quadVAO->Bind();
	shader->Activate();
	texture->Bind();

	glUniformMatrix4fv(glGetUniformLocation(shader->ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void TransferFunction::Bind()
{
	if (texture != nullptr) {
		texture->Bind();
	}
}

void TransferFunction::Unbind()
{
	if (texture != nullptr) {
		texture->Unbind();
	}
}

void TransferFunction::defaultTransferFunction(glm::ivec2 dimensions)
{
	std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dimensions.x * dimensions.y);
	for (int y = 0; y < dimensions.y; y++) {
		for (int x = 0; x < dimensions.x; x++) {
			if (x > 3 && x < 250) {
				bytes[y * dimensions.x + x].x = x / (float)dimensions.x * y / (float)dimensions.y;
				bytes[y * dimensions.x + x].y = x / (float)dimensions.x * x / (float)dimensions.x;
				bytes[y * dimensions.x + x].z = x / (float)dimensions.x * x / (float)dimensions.x;
				bytes[y * dimensions.x + x].w = (std::pow((x - 3), 0.5) <= (float)dimensions.x) ? std::pow((x - 3), 0.5) / (float)dimensions.x : 1.0f;
			}
			else {
				bytes[y * dimensions.x + x].x = 0.0f;
				bytes[y * dimensions.x + x].y = 0.0f;
				bytes[y * dimensions.x + x].z = 0.0f;
				bytes[y * dimensions.x + x].w = 0.0f;
			}
		}
	}
	if (texture != nullptr) {
		delete texture;
	}
	texture = new Texture2D(bytes, dimensions, 1, GL_RGBA, GL_FLOAT);
}

void TransferFunction::spatialTransferFunction(glm::ivec2 dimensions, Texture3D& voxels) {
	std::vector<glm::vec4> bytes = std::vector<glm::vec4>(dimensions.x * dimensions.y);
	for (int i = 0; i < dimensions.x * dimensions.y; i++) {
		bytes[i] = glm::vec4(0.0f);
	}
	int minX = 0.0f * (float)dimensions.x;
	int maxX = 0.98f * (float)dimensions.x;
	int minY = 0.0f * (float)dimensions.y;
	int maxY = 0.98f * (float)dimensions.y;
	Dimensions voxelDim = voxels.getDimensions();
	float divider = voxelDim.width * voxelDim.height * voxelDim.depth * 0.01f;
	for (int z = 0; z < voxelDim.depth; z++) {
		for (int y = 0; y < voxelDim.height; y++) {
			for (int x = 0; x < voxelDim.width; x++) {
				glm::vec4 gradientIntensity = voxels.resampleGradientAndDensity(glm::ivec3(x, y, z));
				int xb = gradientIntensity.w * (dimensions.x - 1);
				int yb = glm::length(glm::vec3(gradientIntensity.x, gradientIntensity.y, gradientIntensity.z)) * (dimensions.y - 1);
				if (xb >= minX && xb <= maxX && yb >= minY && yb <= maxY) {
					bytes[yb * dimensions.x + xb] += glm::vec4(x, y, z, 1.0f) / glm::vec4(voxelDim.width, voxelDim.height, voxelDim.depth, 10);
				}
			}
		}
	}
	if (texture != nullptr) {
		delete texture;
	}
	texture = new Texture2D(bytes, dimensions, 1, GL_RGBA, GL_FLOAT);
}

void TransferFunction::operator=(TransferFunction& transferFunction)
{
	glm::ivec2 dim = transferFunction.getDimensions();
	if (dim.x == 0 || dim.y == 0) {	// Clear if empty.
		if (texture != nullptr) {
			delete texture;
		}
		return;
	}
	std::vector<glm::vec4> bytes(dim.x * dim.y);
	for (int y = 0; y < dim.y; y++) {
		for (int x = 0; x < dim.x; x++) {
			bytes[y * dim.x + x] = transferFunction(glm::ivec2(x, y));
		}
	}
	if (texture != nullptr) {
		delete texture;
	}
	texture = new Texture2D(bytes, dim, 1, GL_RGBA, GL_FLOAT);
}

void TransferFunction::setCamSpacePosition(glm::vec2 camPos)
{
	float aspectRatio = getDimensions().x / (float)getDimensions().y;
	modelMatrix = glm::translate(glm::vec3(camPos.x, camPos.y, 0.0f)) * glm::scale(glm::vec3(0.1f * aspectRatio, 0.1f, 1.0f));
	invModelMatrix = glm::inverse(modelMatrix);

}
