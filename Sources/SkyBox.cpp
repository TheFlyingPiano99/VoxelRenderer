#include "SkyBox.h"
#include<glm/gtc/type_ptr.hpp>

void SkyBox::draw(Camera& camera) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    shader->Activate();
    glm::mat4 view = glm::mat4(glm::mat3(camera.view)); // Remove translation.
    glm::mat4 viewProj = camera.projection * view;
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, "camera.viewProjMatrix"), 1, GL_FALSE, glm::value_ptr(viewProj));
    vao.Bind();
    texture.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
