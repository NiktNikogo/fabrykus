#pragma once

#include "Nodes/simpleMachineNode.hpp"

struct SavedLink {
	size_t leftIdx;
	size_t rightIdx;
	SimpleMachineNode* leftNode;
	SimpleMachineNode* rightNode;
	float weight=0.0f;
};