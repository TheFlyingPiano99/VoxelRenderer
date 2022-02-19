#pragma once
#include "SceneObject.h"
class Moon :
    public SceneObject
{
public:
    
    Moon(Shader* _shader): SceneObject(nullptr, _shader) {
        std::vector<glm::vec3> colors;
        colors.push_back(glm::vec3(0.3f, 0.3f, 0.3f));
        mesh = Mesh::createSphere(1, colors);
    }

};