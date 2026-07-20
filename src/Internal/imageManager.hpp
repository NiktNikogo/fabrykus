#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <imgui.h>

#include "placeHolderData.h"

class ImageManager {
public:
	static ImageManager& get() {
		static ImageManager instance;
		return instance;
	}
	auto loadRegistry(const std::string& path) -> void;
	auto getTexture(const std::string& productName) -> ImTextureID;
	 
	auto addAsset(const std::string& productID, const std::string& path) -> void;
	auto updateAsset(const std::string& productID, const std::string& path) -> void;
	auto removeAsset(const std::string& productID) -> void;
	auto saveRegistry() -> void;
	auto getRegistry() -> const std::unordered_map<std::string, std::string>&;
	auto getCurrentPath() -> const std::string&; 
private:
	ImageManager() {
		init();
	}

	std::unordered_map<std::string, ImTextureID> cache;
	std::unordered_map<std::string, std::string> registry;
	std::string registryPath;

	auto loadTextureFromFile(const std::string& path) -> ImTextureID;
	static auto loadTextureFromMemory(const unsigned char* data, int size) -> ImTextureID;
	auto init() -> void;
};


