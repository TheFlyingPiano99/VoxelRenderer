#include "SingletonManager.h"

#include "ControlActionManager.h"
#include "Scene.h"
#include "AssetManager.h"
#include "GUI.h"

/*
* Needs to be called before closing program!
*/
void Singleton::destroyAllInstances() {
	ControlActionManager::destroyInstance();
	Scene::destroyInstance();
	AssetManager::destroyInstance();
	GUI::destroyInstance();
}
