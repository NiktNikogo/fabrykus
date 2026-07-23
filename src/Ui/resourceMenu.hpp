#pragma once

#include <string>
#include <imgui.h>
#include "Internal/imageManager.hpp"

class ResourceMenu {
private:
	bool isHiddenByKeys;
	ImageManager& manager;

public:
	const float menuWidth = 500.0f;
	const float menuHeight = 25.0f;

public:
	ResourceMenu(ImageManager& manager)
		: 
		isHiddenByKeys(true),
		manager(manager)
	{}

	const auto draw() -> void const;
	const auto update() -> void;

	auto setHiddenByKeys(bool isHidden) -> void;
	const auto getHiddenByKeys() -> bool const;
};
