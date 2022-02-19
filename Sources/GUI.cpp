#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GUI* GUI::instance = nullptr;

void GUI::initGUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void GUI::destroyGUI() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GUI::preDrawInit()
{
	if (!visible) {
		return;
	}
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

}

void GUI::configToScene(Scene& scene)
{
	if (!visible) {
		return;
	}
	ImGui::Begin("Atmosphere properties");

	float minGamma = 0;
	float maxGamma = 10;
	float minExposure = 0;
	float maxExposure = 1;

	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("HDR");
	ImGui::SliderFloat("Gamma", scene.getPostprocessUnit()->getGamma(), minGamma, maxGamma);
	ImGui::SliderFloat("Exposure", scene.getPostprocessUnit()->getExposure(), minExposure, maxExposure);
	ImGui::EndChild();

	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("Rayleigh scattering");
	float minRayleigh = 0;
	float maxRayleigh = 0.5f;
	ImGui::SliderFloat("Rayleigh R", &(scene.getPlanet()->getRayleighScattering()->x), minRayleigh, maxRayleigh);
	ImGui::SliderFloat("Rayleigh G", &(scene.getPlanet()->getRayleighScattering()->y), minRayleigh, maxRayleigh);
	ImGui::SliderFloat("Rayleigh B", &(scene.getPlanet()->getRayleighScattering()->z), minRayleigh, maxRayleigh);
	ImGui::EndChild();

	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("Mie scattering");
	float minMie = 0;
	float maxMie = 0.02;
	ImGui::SliderFloat("Mie", scene.getPlanet()->getMieScattering(), minMie, maxMie);
	ImGui::EndChild();

	/*
	* 	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("Density");
	float minDensity = -2;
	float maxDensity = 2;
	ImGui::SliderFloat("Quadratic", scene.getPlanet()->getQuadraticDensity(), minDensity, maxDensity);
	ImGui::SliderFloat("Linear", scene.getPlanet()->getLinearDensity(), minDensity, maxDensity);
	ImGui::SliderFloat("Constant", scene.getPlanet()->getConstantDensity(), minDensity, maxDensity);
	ImGui::EndChild();

	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("Light absorption");
	ImGui::Text("Quadratic");
	float minAbsorption = 0;
	float maxAbsorption = 10;
	glm::vec3* quadraticAbsoprtion = scene.getPlanet()->getQuadraticAbsorption();
	ImGui::SliderFloat("Red_AQ", &(quadraticAbsoprtion->x), minAbsorption, maxAbsorption);
	ImGui::SliderFloat("Green_AQ", &(quadraticAbsoprtion->y), minAbsorption, maxAbsorption);
	ImGui::SliderFloat("Blue_AQ", &(quadraticAbsoprtion->z), minAbsorption, maxAbsorption);
	ImGui::Text("Linear");
	glm::vec3* linearAbsoprtion = scene.getPlanet()->getLinearAbsorption();
	ImGui::SliderFloat("Red_AL", &(linearAbsoprtion->x), minAbsorption, maxAbsorption);
	ImGui::SliderFloat("Green_AL", &(linearAbsoprtion->y), minAbsorption, maxAbsorption);
	ImGui::SliderFloat("Blue_AL", &(linearAbsoprtion->z), minAbsorption, maxAbsorption);
	ImGui::Text("Constant");
	glm::vec3* constantAbsorption = scene.getPlanet()->getConstantAbsorption();
	ImGui::SliderFloat("Red_AC", &(constantAbsorption->x), minAbsorption, maxAbsorption);
	ImGui::SliderFloat("Green_AC", &(constantAbsorption->y), minAbsorption, maxAbsorption);
	ImGui::SliderFloat("Blue_AC", &(constantAbsorption->z), minAbsorption, maxAbsorption);
	ImGui::EndChild();

	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("Light scattering");
	ImGui::Text("Quadratic");
	float minScattering = 0;
	float maxScattering = 10;
	glm::vec3* quadraticScattering = scene.getPlanet()->getQuadraticScattering();
	ImGui::SliderFloat("Red_SQ", &(quadraticScattering->x), minScattering, maxScattering);
	ImGui::SliderFloat("Green_SQ", &(quadraticScattering->y), minScattering, maxScattering);
	ImGui::SliderFloat("Blue_SQ", &(quadraticScattering->z), minScattering, maxScattering);
	ImGui::Text("Linear");
	glm::vec3* linearScattering = scene.getPlanet()->getLinearScattering();
	ImGui::SliderFloat("Red_SL", &(linearScattering->x), minScattering, maxScattering);
	ImGui::SliderFloat("Green_SL", &(linearScattering->y), minScattering, maxScattering);
	ImGui::SliderFloat("Blue_SL", &(linearScattering->z), minScattering, maxScattering);
	ImGui::Text("Constant");
	glm::vec3* constantScattering = scene.getPlanet()->getConstantScattering();
	ImGui::SliderFloat("Red_SC", &(constantScattering->x), minScattering, maxScattering);
	ImGui::SliderFloat("Green_SC", &(constantScattering->y), minScattering, maxScattering);
	ImGui::SliderFloat("Blue_SC", &(constantScattering->z), minScattering, maxScattering);
	ImGui::EndChild();

	ImGui::BeginChild("Atmosphere properties");
	ImGui::Text("Reflectiveness");
	ImGui::Text("Quadratic");
	float minReflectiveness = 0;
	float maxReflectiveness = 10;
	glm::vec3* quadraticReflectiveness = scene.getPlanet()->getQuadraticReflectiveness();
	ImGui::SliderFloat("Red_RQ", &(quadraticReflectiveness->x), minReflectiveness, maxReflectiveness);
	ImGui::SliderFloat("Green_RQ", &(quadraticReflectiveness->y), minReflectiveness, maxReflectiveness);
	ImGui::SliderFloat("Blue_RQ", &(quadraticReflectiveness->z), minReflectiveness, maxReflectiveness);
	ImGui::Text("Linear");
	glm::vec3* linearReflectiveness = scene.getPlanet()->getLinearReflectiveness();
	ImGui::SliderFloat("Red_RL", &(linearReflectiveness->x), minReflectiveness, maxReflectiveness);
	ImGui::SliderFloat("Green_RL", &(linearReflectiveness->y), minReflectiveness, maxReflectiveness);
	ImGui::SliderFloat("Blue_RL", &(linearReflectiveness->z), minReflectiveness, maxReflectiveness);
	ImGui::Text("Constant");
	glm::vec3* constantReflectiveness = scene.getPlanet()->getConstantReflectiveness();
	ImGui::SliderFloat("Red_RC", &(constantReflectiveness->x), minReflectiveness, maxReflectiveness);
	ImGui::SliderFloat("Green_RC", &(constantReflectiveness->y), minReflectiveness, maxReflectiveness);
	ImGui::SliderFloat("Blue_RC", &(constantReflectiveness->z), minReflectiveness, maxReflectiveness);
	ImGui::EndChild();

	*/

	ImGui::End();
}


void GUI::draw()
{
	if (!visible) {
		return;
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
