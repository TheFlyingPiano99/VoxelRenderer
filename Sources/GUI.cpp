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

void GUI::configToScene(Scene& scene)
{
	if (!visible)
		return;
	char value_buf[64] = {};
	ImGui::Begin("Settings");
	ImGui::SliderFloat("Exposure", &scene.getVoxelData()->getExposure(), 0.0f, 2.0f);
	ImGui::SliderFloat("Gamma", &scene.getVoxelData()->getGamma(), 0.0f, 1.0f);

	ImGui::SliderFloat("STF Exposure", &scene.getVoxelData()->getReferenceTransferFunctionExposure(), 0.0f, 50.0f);
	ImGui::SliderFloat("STF Gamma", &scene.getVoxelData()->getReferenceTransferFunctionGamma(), 0.0f, 10.0f);

	ImGui::SliderFloat("Bounding geometry threshold", &scene.getVoxelData()->getBoundingGeometryThreshold(), 0.0f, 1.0f);
	ImGui::SliderFloat("Transfer function flood fill threshold", &scene.getVoxelData()->getTransferFloodFillThreshold(), 0.0f, 5.0f);

	const char* current_item = scene.getVoxelData()->getCurrentTransferRegionSelectModes();
	const char** items = scene.getVoxelData()->getTransferRegionSelectModes();
	if (ImGui::BeginCombo("Transfer region select mode", current_item)) // The second parameter is the label previewed before opening the combo.
	{
		for (int n = 0; n < 2; n++)
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
	ImGui::End();
}


void GUI::draw()
{
	if (!visible)
		return;
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
