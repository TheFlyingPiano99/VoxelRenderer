#pragma once

#include "Mesh.h"
#include "shaderClass.h"
#include "Camera.h"
#include "Light.h"
#include "Animation.h"
#include "ISceneObject.h"

/*
* Abstract parent of all displayable objects
*/
class SceneObject : public ISceneObject
{
protected:
	Mesh* mesh = nullptr;		// Don't delete!
	Shader* shader = nullptr;	// Don't delete!

	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec3 eulerAngles = glm::vec3(0.0f);
	glm::vec3 position = glm::vec3(0.0f);

	Animation* animation = nullptr;

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	glm::mat4 invModelMatrix = glm::mat4(1.0f);

	/*
	* Exports model matrix into shader uniform
	*/
	void exportMatrix();

public:
	SceneObject(Mesh* _mesh = nullptr, Shader* _shader = nullptr) : mesh(_mesh), shader(_shader) {
	}

	~SceneObject() {
	}

	void update();

	virtual void control(float dt) {}

	virtual void animate(float dt);

	virtual void draw(Camera& camera, std::vector<Light>& lights);

	void setMesh(Mesh* _mesh) {
		mesh = _mesh;
	}
	void setShader(Shader* _shader) {
		shader = _shader;
	}

	void setAnimation(Animation* _animation) {
		animation = _animation;
	}

	void setPosition(glm::vec3 pos) override {
		position = pos;
	}

	glm::vec3 getPosition() {
		return position;
	}

	void setScale(glm::vec3 s) {
		scale = s;
	}

	glm::vec3 getScale() {
		return scale;
	}

	void setEulerAngles(glm::vec3 angles) {
		eulerAngles = angles;
	}
};

