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

	AssetManager() {
	}

	~AssetManager() {
	}

	std::string findPathIntoFolder(std::string folderName) {
		std::string path = folderName;
		path.append("/");
		std::filesystem::path p = std::filesystem::path(folderName.c_str());
		int i = 0;
		while (!std::filesystem::is_directory(p)) {
			if (i > 5) {
				std::string errorMessage = "Folder <";
				errorMessage.append(folderName);
				errorMessage.append("> not found!");
				throw std::exception(errorMessage.c_str());
			}
			path.clear();
			for (int j = 0; j < i; j++) {
				path = path.append("../");
			}
			path = path.append(folderName);
			path.append("/");
			p = std::filesystem::path(path.c_str());
			i++;
		}
		return path;
	}


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

	std::string getShaderFolderPath() {
		if (shaderFolderPath.empty()) {
			shaderFolderPath = findPathIntoFolder("Resources/Shaders");
		}
		return shaderFolderPath;
	}

	std::string getTextureFolderPath() {
		if (textureFolderPath.empty()) {
			textureFolderPath = findPathIntoFolder("Resources/Textures");
		}
		return textureFolderPath;
	}

};