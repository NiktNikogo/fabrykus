#pragma once

#include <vector>

#include "Util/ingredient.hpp"

struct InspectorConfig {
	bool showAmmount = true;
	bool allowAdding = true;
	bool allowDeleting = true;
	bool sameName = false;
	std::vector<Ingredient>* keeps = nullptr;
};