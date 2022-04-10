#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GlobalInclude.h"

GUI* GUI::instance = nullptr;

void GUI::initGUI(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, false);
	ImGui_ImplOpenGL3_Init("#version 420");
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

static bool configFeature = false;
static bool configSTF = false;

void GUI::configToScene(Scene& scene)
{
	if (!visible)
		return;
	char value_buf[64] = {};
	ImGui::Begin("Settings");
	ImGui::SliderFloat("Exposure", &scene.getVoxelData()->getExposure(), 0.0f, 2.0f);
	ImGui::SliderFloat("Gamma", &scene.getVoxelData()->getGamma(), 0.0f, 1.0f);
	ImGui::SliderFloat("Light power", &scene.getLightsPower(), 100.0f, 100000.0f);

	ImGui::SliderFloat("Bounding geometry threshold", &scene.getVoxelData()->getBoundingGeometryThreshold(), 0.0f, 0.01f);
	ImGui::SliderFloat("Transfer function flood fill threshold", &scene.getVoxelData()->getTransferFloodFillThreshold(), 0.0f, 5.0f);

	const char* current_item = scene.getVoxelData()->getCurrentTransferRegionSelectModes();
	const char** items = scene.getVoxelData()->getTransferRegionSelectModes();
	if (ImGui::BeginCombo("Transfer region select mode", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < TRANSFER_MODE_COUNT; n++)
		{
			bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(items[n], is_selected)) {
				current_item = items[n];
				scene.getVoxelData()->setCurrentTransferRegionSelectModes(current_item);
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}

	current_item = (scene.getVoxelData()->getSelectedFeature() != nullptr) ?
		scene.getVoxelData()->getSelectedFeature()->name.c_str() : "Select feature";
	std::vector<Feature> features = scene.getVoxelData()->getTransferFunction()->getFeatures();

	if (ImGui::BeginCombo("Feature", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; 
			n < scene.getVoxelData()
			->getTransferFunction()
			->getFeatures().size();
			n++)
		{
			bool is_selected = (current_item == features[n].name.c_str()); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(features[n].name.c_str(), is_selected)) {
				current_item = features[n].name.c_str();
				scene.getVoxelData()->setSelectedFeature(current_item);
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
		}
		ImGui::EndCombo();
	}

	ImGui::BeginGroup();
	if (ImGui::Button("Reset to STF", ImVec2(120, 50))) {
		configSTF = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset to Default", ImVec2(120, 50))) {
		scene.getVoxelData()->resetToDefault();
	}
	ImGui::SameLine();
	if (ImGui::Button("Config feature", ImVec2(120, 50))) {
		configFeature = true;
	}
	if (ImGui::Button("Show all features", ImVec2(120, 50))) {
		scene.getVoxelData()->showAll();
	}
	if (ImGui::Button("Load features", ImVec2(120, 50))) {
		scene.getVoxelData()->loadFeatures();
	}
	ImGui::EndGroup();
	ImGui::BeginGroup();
	ImGui::Text("Rotate");
	if (ImGui::Button("X", ImVec2(25, 25))) {
		scene.getVoxelData()->rotateModelAroundX(M_PI / 2.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Y", ImVec2(25, 25))) {
		scene.getVoxelData()->rotateModelAroundY(M_PI / 2.0f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Z", ImVec2(25, 25))) {
		scene.getVoxelData()->rotateModelAroundZ(M_PI / 2.0f);
	}
	ImGui::EndGroup();
	ImGui::End();

	if ( configFeature && scene.getVoxelData()->getSelectedFeature() != nullptr ) {
		ImGui::Begin("Config feature");
		ImGui::ColorEdit3("Color", &scene.getVoxelData()->getSelectedFeature()->color.x);
		ImGui::SliderFloat("Opacity", &scene.getVoxelData()->getSelectedFeature()->opacity, 0.0f, 1.0f);
		ImGui::SliderFloat("Emission", &scene.getVoxelData()->getSelectedFeature()->emission, 0.0f, 2.0f);
		if (ImGui::Button("Finish", ImVec2(120, 50))) {
			scene.getVoxelData()->redrawSelected();
			configFeature = false;
		}

		ImGui::End();
	}

	if (configSTF) {
		ImGui::Begin("Configure STF");

		ImGui::SliderFloat("Class radius", &scene.getVoxelData()->getSTFradius(), 0.0f, 50.0f);
		ImGui::SliderFloat("Global opacity", &scene.getVoxelData()->getSTFOpacity(), 0.0f, 50.0f);
		ImGui::SliderFloat("Global emission", &scene.getVoxelData()->getSTFEmission(), 0.0f, 2.0f);

		if (ImGui::Button("Generate", ImVec2())) {
			scene.getVoxelData()->resetToSTF();
			configSTF = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2())) {
			configSTF = false;
		}
	}
}


void GUI::draw()
{
	if (!visible)
		return;
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
