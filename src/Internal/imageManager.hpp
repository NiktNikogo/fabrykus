#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <imgui.h>

#include "placeHolderData.h"
#include "Util/texture.hpp"

class ImageManager {
public:
	static ImageManager& get() {
		static ImageManager instance;
		return instance;
	}
	auto loadRegistry(const std::string& path) -> void;
	auto getTexture(const std::string& productName) -> Texture;
	auto addAsset(const std::string& productID, const std::string& path) -> void;
	auto updateAsset(const std::string& productID, const std::string& path) -> void;
	auto removeAsset(const std::string& productID) -> void;
	auto saveRegistry() -> void;
	auto getRegistry() -> const std::unordered_map<std::string, std::string>&;
	auto getCurrentPath() -> const std::string&;
	auto isInRegistry(const std::string& key) -> bool; 
private:
	ImageManager() {
		init();
	}

	std::unordered_map<std::string, Texture> cache;
	std::unordered_map<std::string, ImVec2> sizes;
	std::unordered_map<std::string, std::string> registry;
	std::string registryPath;


	auto loadTextureFromFile(const std::string& path) -> Texture;
	static auto loadTextureFromMemory(const unsigned char* data, int size) -> Texture;
	auto init() -> void;
};


