#include "Texture3D.h"

Texture3D::Texture3D(const char* image, GLuint slot, GLenum format, GLenum pixelType)
{
	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, depthImg, numColCh;
	widthImg = 100;
	heightImg = 100;
	depthImg = 100;
	numColCh = 4;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = new unsigned char[100 * 100 * 100 * 4];
	for (int x = 0; x < widthImg; x++) {
		for (int y = 0; y < heightImg; y++) {
			for (int z = 0; z < depthImg; z++) {
				int coord = x * heightImg * depthImg + y * depthImg + z;
				float r = (x - 50) * (x - 50) + (y - 50) * (y - 50) + (z - 50) * (z - 50);
				float r2 = (x - 50) * (x - 50) + (y - 50) * (y - 50);
				if (r < 50 * 50 && r2 < 25 * 25) {	// F(x)
					bytes[coord * numColCh] = x;
					bytes[coord * numColCh + 1] = y;
					bytes[coord * numColCh + 2] = z;
					bytes[coord * numColCh + 3] = 1;
				}
				else {
					bytes[coord * numColCh] = 0;
					bytes[coord * numColCh + 1] = 0;
					bytes[coord * numColCh + 2] = 0;
					bytes[coord * numColCh + 3] = 1;
				}
			}
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
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 100, 100, 100, 0, format, pixelType, bytes);
	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_3D);

	// Deletes the image data as it is already in the OpenGL Texture object
	//stbi_image_free(bytes);
	delete[] bytes;

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_3D, 0);
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