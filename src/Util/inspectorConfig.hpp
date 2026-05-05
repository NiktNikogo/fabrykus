#pragma once

#include <vector>

#include "Util/ingredient.hpp"

struct InspectorConfig {
	bool showAmmount = true;
	bool allowAdding = true;
	bool allowDeleting = true;
	bool sameName = false;
	std::vector<Ingredient>* keeps = nullptr;
	bool capInputValues = false;
	Rational cappedValue = Rational(LLONG_MAX, 1);	
};