#pragma once

#include "Scene.h"
#include <queue>
#include <GLFW/glfw3.h>

class ControlAction
{
	int key;
	bool enableRepeat;

public:
	ControlAction(int _key, bool _enableRepeat = true) : key(_key), enableRepeat(_enableRepeat) {
	}

	bool isThisAction(int key, int scancode, int action);

	virtual void execute(Scene* scene, float dt) = 0;
};

class MoveCameraForward : public ControlAction {
public:
	MoveCameraForward() : ControlAction(GLFW_KEY_W) {

	}

	void execute(Scene* scene, float dt) override;
};

class MoveCameraBackward : public ControlAction {
public:
	MoveCameraBackward() : ControlAction(GLFW_KEY_S) {
	}

	void execute(Scene* scene, float dt) override;
};

class ToggleGUI : public ControlAction {
public:
	ToggleGUI() : ControlAction(GLFW_KEY_O, false) {
	}

	void execute(Scene* scene, float dt) override;
};

class FastForward : public ControlAction {
public:
	FastForward() : ControlAction(GLFW_KEY_F) {
	}

	void execute(Scene* scene, float dt) override;
};

class Rewind : public ControlAction {
public:
	Rewind() : ControlAction(GLFW_KEY_R) {
	}

	void execute(Scene* scene, float dt) override;
};

class TogglePause : public ControlAction {
public:
	TogglePause() : ControlAction(GLFW_KEY_P, false) {
	}

	void execute(Scene* scene, float dt) override;
};

class ToggleGravitation : public ControlAction {
public:
	ToggleGravitation() : ControlAction(GLFW_KEY_G, false) {
	}

	void execute(Scene* scene, float dt) override;
};

class ShiftIntersectionPlanePos : public ControlAction {
public:
	ShiftIntersectionPlanePos() : ControlAction(GLFW_KEY_M, true) {
	}

	void execute(Scene* scene, float dt) override;
};

class ShiftIntersectionPlaneNeg : public ControlAction {
public:
	ShiftIntersectionPlaneNeg() : ControlAction(GLFW_KEY_N, true) {
	}

	void execute(Scene* scene, float dt) override;
};

class RotateIntersectionPlanePosY : public ControlAction {
public:
	RotateIntersectionPlanePosY() : ControlAction(GLFW_KEY_L, true) {
	}

	void execute(Scene* scene, float dt) override;
};

class RotateIntersectionPlaneNegY : public ControlAction {
public:
	RotateIntersectionPlaneNegY() : ControlAction(GLFW_KEY_K, true) {
	}

	void execute(Scene* scene, float dt) override;
};

class ToggleFullScreenMode : public ControlAction {
public:
	ToggleFullScreenMode() : ControlAction(GLFW_KEY_TAB, true) {
	}

	void execute(Scene* scene, float dt) override;
};

class CycleFeatures : public ControlAction {
public:
	CycleFeatures() : ControlAction(GLFW_KEY_SPACE, false) {
	}

	void execute(Scene* scene, float dt) override;
};

class ToggleTransferFunction : public ControlAction {
public:
	ToggleTransferFunction() : ControlAction(GLFW_KEY_T, false) {
	}

	void execute(Scene* scene, float dt) override;
};
