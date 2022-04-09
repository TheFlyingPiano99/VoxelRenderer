#pragma once

#include <string>
#include <fstream>
#include <filesystem>


/*
* Singleton
*/
class AssetManager {
	static AssetManager* instance;

	std::string shaderFolderPath;
	std::string textureFolderPath;
	std::string savesFolderPath;

	AssetManager() {
	}

	~AssetManager() {
	}

	std::string findPathIntoFolder(std::string folderName);


public:

	static AssetManager* getInstance() {
		if (instance == nullptr) {
			instance = new AssetManager();
		}
		return instance;
	}

	static void destroyInstance() {
		if (instance != nullptr) {
			delete instance;
			instance = nullptr;
		}
	}

	std::string getShaderFolderPath();

	std::string getTextureFolderPath();

	std::string getSavesFolderPath();
};