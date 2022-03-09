#include "Texture3D.h"
#include <iomanip>
#include <vector>

Texture3D::Texture3D(const char* directory, const Dimensions dimensions, GLuint slot, GLenum format, GLenum pixelType)
	: dimensions(dimensions)
{

	bytes = new unsigned char[dimensions.width * dimensions.height * dimensions.depth * dimensions.bytesPerVoxel];

	// Flips the image so it appears right side up
	//stbi_set_flip_vertically_on_load(false);

	// Reads the image from a file and stores it in bytes
	for (int z = 0; z < dimensions.depth; z++) {
		std::stringstream pathss;
		pathss << directory << std::setw(3) << std::setfill('0') << z + 1 << ".tif";	// <directory/><number of image>.tif
		std::string path = pathss.str();
		FILE* file;
		errno_t err;
		//unsigned char* imageBytes = stbi_load(path.c_str(), &widthImg, &heightImg, &numColCh, 0);
		if (err = fopen_s(&file, path.c_str(), "rb") == 0) {
			int bytesCount = fread(bytes + z * dimensions.width * dimensions.height * dimensions.bytesPerVoxel, sizeof(char), dimensions.width * dimensions.height * dimensions.bytesPerVoxel, file);
			fclose(file);
		}
	}

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_3D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Assigns the image to the OpenGL Texture object
	//TODO
	glTexImage3D(GL_TEXTURE_3D, 0, format, dimensions.width, dimensions.height, dimensions.depth, 0, format, pixelType, bytes);
	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_3D);

	// Deletes the image data as it is already in the OpenGL Texture object

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_3D, 0);
}

Texture3D::~Texture3D()
{
	if (bytes != nullptr) {
		delete[] bytes;
	}
}

void Texture3D::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();

	// Sets the value of the uniform
	glUniform1i(texUni, unit);
	glUniform1f(glGetUniformLocation(shader.ID, "shininess"), shininess);
}

void Texture3D::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_3D, ID);
}

void Texture3D::Unbind()
{
	glBindTexture(GL_TEXTURE_3D, 0);
}

void Texture3D::Delete()
{
	glDeleteTextures(1, &ID);
}