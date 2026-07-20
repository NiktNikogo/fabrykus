#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <imgui.h>

class ImageManager {

public:
	static ImageManager& get() {
		static ImageManager instance;
		instance.registryPath = "./src/res.json";
		instance.addAsset("placeholder", "./src/placeholder.png");
		instance.getTexture("placeholder");
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
	ImageManager() = default;

	std::unordered_map<std::string, ImTextureID> cache;
	std::unordered_map<std::string, std::string> registry;
	std::string registryPath;

	ImTextureID loadTextureFromFile(const std::string& path);
};


