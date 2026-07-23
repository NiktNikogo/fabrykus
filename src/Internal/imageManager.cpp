#include "imageManager.hpp"
#include <fstream>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <GLFW/glfw3.h>

auto ImageManager::loadRegistry(const std::string &path) -> void
{
	registryPath = path;
	std::ifstream file(path);
	std::cout << path << '\n';
	if(!file.is_open()) return;

	nlohmann::json j;
	file >> j;

	registry = j.get<std::unordered_map<std::string, std::string>>();

}

auto ImageManager::getTexture(const std::string &productName) -> Texture
{
	if(cache.count(productName)) {
		return cache[productName];
	}

	if(registry.count(productName)) {
		auto tex = loadTextureFromFile(registry[productName]);
		cache[productName] = tex;
		return tex;
	}

	return cache["placeholder"];
}

auto ImageManager::addAsset(const std::string &productID, const std::string &path) -> void
{
	registry[productID] = path;
	if(cache.count(productID)) {
		cache.erase(productID);
	}
	saveRegistry();
}

auto ImageManager::updateAsset(const std::string &productID, const std::string &path) -> void
{
	registry[productID] = path;
	if(cache.count(productID)) {
		cache.erase(productID);
		cache[productID] = loadTextureFromFile(registry[productID]);
		
	}
	saveRegistry();
}

auto ImageManager::removeAsset(const std::string &productID) -> void
{
	registry.erase(productID);
	cache.erase(productID);
	saveRegistry();
}

auto ImageManager::saveRegistry() -> void
{
	std::ofstream file(registryPath);
	nlohmann::json j = registry;
	file << j.dump(4);
}

auto ImageManager::getRegistry() -> const std::unordered_map<std::string, std::string> &
{
	return registry;
}

auto ImageManager::getCurrentPath() -> const std::string &
{
	return registryPath;
}

auto ImageManager::isInRegistry(const std::string& key) -> bool
{
	return registry.find(key) != registry.end();
}

auto ImageManager::loadTextureFromFile(const std::string &path) -> Texture
{
	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if(!data) return {0, 0, 0};

	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	return {width, height, texId};
}

auto ImageManager::loadTextureFromMemory(const unsigned char *data, int size) -> Texture
{
	int width, height, channels;
	if(!data || size <= 0) return {0, 0, 0};
	unsigned char* imgData = stbi_load_from_memory(data, size, &width, &height, &channels, 4);
	if(!imgData) return {0, 0, 0};

	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

	stbi_image_free(imgData);
	return {width, height, texId};
}

auto ImageManager::init() -> void
{
	registryPath = "./res.json";

	if(std::filesystem::exists(registryPath)) {
		loadRegistry(registryPath);
		saveRegistry();
		return;
	}

	std::string placeholderPath = "./placeholder.png";
	registry["placeholder"] = placeholderPath;
	
	if(!std::filesystem::exists(placeholderPath)) {
		std::ofstream outFile(placeholderPath, std::ios::binary);
		if(outFile.is_open()) {
			outFile.write(reinterpret_cast<const char*>(placeholderData), placeholderLength);
			outFile.close();
		}
		cache["placeholder"] = ImageManager::loadTextureFromMemory(placeholderData, placeholderLength);
	} else {
		cache["placeholder"] = ImageManager::loadTextureFromFile(placeholderPath);
	}
	
	loadRegistry(registryPath);	
	saveRegistry();
}
