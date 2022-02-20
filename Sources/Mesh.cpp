#include "Mesh.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture2D>& textures)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	EBO.Unbind();
}


void Mesh::Draw(Shader& shader, Camera& camera)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	VAO.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}
	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.exportMatrix(shader);

	
	glUniform1i(glGetUniformLocation(shader.ID, "useTexture"), textures.size() > 0);

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

Mesh* Mesh::createSphere(float r, std::vector<glm::vec3> colors)
{
	// ------------------- ICOSAHEDRON SPHERE -------------------

	float a = 3.0f;
	float b = 1.0f;
	float sum = a + b;

	std::vector<Vertex> vertices;

	srand(0);
	glm::vec2 texture = glm::vec2(0.0f, 0.0f);

	glm::vec3 norm = glm::vec3(1.0f, 1.0f, 1.0f);

	std::vector<glm::vec3> positions;

	// base poits of rectangles:

	// x rectangle:
	glm::vec3 x1 = glm::vec3(0, sum / 2, a / 2);
	glm::vec3 x2 = glm::vec3(0, -sum / 2, a / 2);
	glm::vec3 x3 = glm::vec3(0, -sum / 2, -a / 2);
	glm::vec3 x4 = glm::vec3(0, sum / 2, -a / 2);

	positions.push_back(x1);
	positions.push_back(x2);
	positions.push_back(x3);
	positions.push_back(x4);

	// y rectangle:
	glm::vec3 y1 = glm::vec3(a / 2, 0, sum / 2);
	glm::vec3 y2 = glm::vec3(a / 2, 0, -sum / 2);
	glm::vec3 y3 = glm::vec3(-a / 2, 0, -sum / 2);
	glm::vec3 y4 = glm::vec3(-a / 2, 0, sum / 2);

	positions.push_back(y1);
	positions.push_back(y2);
	positions.push_back(y3);
	positions.push_back(y4);

	//z rectangle:
	glm::vec3 z1 = glm::vec3(sum / 2, a / 2, 0);
	glm::vec3 z2 = glm::vec3(-sum / 2, a / 2, 0);
	glm::vec3 z3 = glm::vec3(-sum / 2, -a / 2, 0);
	glm::vec3 z4 = glm::vec3(sum / 2, -a / 2, 0);

	positions.push_back(z1);
	positions.push_back(z2);
	positions.push_back(z3);
	positions.push_back(z4);

	std::vector<GLuint> indices;

	int resolution = 10;

	// generates points on sphere from icosaheder faces
	generateIcosaFace(x1, y1, y4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x2, y1, y4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x3, y2, y3, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x4, y2, y3, resolution, 3.0f, &positions, &indices);

	generateIcosaFace(y1, z1, z4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(y2, z1, z4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(y3, z2, z3, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(y4, z2, z3, resolution, 3.0f, &positions, &indices);

	generateIcosaFace(z1, x1, x4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(z2, x1, x4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(z3, x2, x3, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(z4, x2, x3, resolution, 3.0f, &positions, &indices);

	generateIcosaFace(x1, y1, z1, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x4, y2, z1, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x1, y4, z2, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x4, y3, z2, resolution, 3.0f, &positions, &indices);

	generateIcosaFace(x2, y1, z4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x3, y2, z4, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x2, y4, z3, resolution, 3.0f, &positions, &indices);
	generateIcosaFace(x3, y3, z3, resolution, 3.0f, &positions, &indices);

	for (int i = 0; i < positions.size(); i++)
	{
		Vertex vert;
		vert.position = positions.at(i);
		vert.normal = normalize(positions.at(i));
		if (colors.size() > 0) {
			int colorIdx = rand() % colors.size();
			glm::vec3 pickedColor = colors[colorIdx];
			vert.color = pickedColor;
		}
		else {
			vert.color = glm::vec3(0.0f);
		}
		vert.texUV = texture;

		vertices.push_back(vert);
	}

	std::vector<Texture2D> tex;

	return new Mesh(vertices, indices, tex);
}

void Mesh::generateIcosaFace(glm::vec3 a, glm::vec3 b, glm::vec3 c, int resolution, float r, std::vector<glm::vec3>* vertices, std::vector<GLuint>* indices)
{
	a = normalize(a);
	a = r * a;
	b = normalize(b);
	b = r * b;
	c = normalize(c);
	c = r * c;

	glm::vec3 aToB = (b - a) / (float)(resolution + 1.0f);
	glm::vec3 bToC = (c - b) / (float)(resolution + 1.0f);

	int vertexIndex = vertices->size();
	int startIndex = vertexIndex;

	vertices->push_back(a);

	for (int i = 1; i < resolution + 2; i++)
	{
		glm::vec3 iterationBase = a + ((float)i * aToB);

		for (int j = 0; j < i + 1; j++)
		{
			glm::vec3 currentVector = iterationBase + ((float)j * bToC);
			currentVector = (r / length(currentVector)) * currentVector;
			vertices->push_back(currentVector);
		}
	}

	for (int i = 1; i < resolution + 2; i++)
	{
		for (int j = 0; j < i; j++)
		{
			vertexIndex++;

			indices->push_back(vertexIndex - i);
			indices->push_back(vertexIndex);
			indices->push_back(vertexIndex + 1);

			if (j != 0)
			{
				indices->push_back(vertexIndex - i);
				indices->push_back(vertexIndex - i - 1);
				indices->push_back(vertexIndex);
			}
		}

		vertexIndex++;
	}
}
