#include "ControlAction.h"
#include "Callbacks.h"
#include "GUI.h"

bool ControlAction::isThisAction(int key, int scancode, int action)
{
	return (this->key == key
		&& (
			(enableRepeat && (action == GLFW_PRESS || action == GLFW_REPEAT))
		|| (!enableRepeat && action == GLFW_PRESS)
			));
}

//---------------------------------------------------------------------------

void MoveCameraForward::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveForward(dt);
	}
}

void MoveCameraBackward::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveBackward(dt);
	}
}

void MoveCameraLeft::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveLeft(dt);
	}
}

void MoveCameraRight::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveRight(dt);
	}
}

void MoveCameraUp::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveUp(dt);
	}
}

void MoveCameraDown::execute(Scene* scene, float dt)
{
	Camera* camera = scene->getCamera();
	if (nullptr != camera) {
		camera->moveDown(dt);
	}
}

void ToggleGUI::execute(Scene* scene, float dt)
{
	GUI::getInstance()->setVisible(!(GUI::getInstance()->isVisible()));
}

void FastForward::execute(Scene* scene, float dt)
{
	scene->animate(dt * 100.0f);
}

void Rewind::execute(Scene* scene, float dt)
{
	scene->animate(dt * -100.0f);
}

void TogglePause::execute(Scene* scene, float dt)
{
	scene->togglePause();
}

void ToggleGravitation::execute(Scene* scene, float dt)
{
	scene->toggleGravitation();
}

void ShiftIntersectionPlanePos::execute(Scene* scene, float dt)
{
	scene->getVoxelData()->shiftIntersectionPlane(0.1f * dt);
}

void ShiftIntersectionPlaneNeg::execute(Scene* scene, float dt)
{
	scene->getVoxelData()->shiftIntersectionPlane(-0.1f * dt);
}

void RotateIntersectionPlanePosY::execute(Scene* scene, float dt)
{
	scene->getVoxelData()->rotateIntersectionPlane(0.01f * dt);
}

void RotateIntersectionPlaneNegY::execute(Scene* scene, float dt)
{
	scene->getVoxelData()->rotateIntersectionPlane(-0.01f * dt);
}
