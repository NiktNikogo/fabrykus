#pragma once

#include <vector>
#include <memory>
#include <imgui.h>
#include "ImNodeFlow.h"
#include "Nodes/simpleMachineNode.hpp"

class IsolatedGraph {
private:
	
	using Id = ImFlow::NodeUID;
	std::vector<Id> ids;
	std::vector<std::shared_ptr<SimpleMachineNode>> sources;
	std::vector<std::shared_ptr<SimpleMachineNode>> nodes;
	std::vector<std::shared_ptr<SimpleMachineNode>> targets;
public:
	IsolatedGraph();
	IsolatedGraph(std::vector<Id> ids);
	auto findSources(ImFlow::ImNodeFlow &grid) -> void;
	auto isProductionLine() const -> bool;
	inline const auto getNodes() -> std::vector<std::shared_ptr<SimpleMachineNode>> {
		return nodes;
	}
	inline const auto getSources() -> std::vector<std::shared_ptr<SimpleMachineNode>> {
		return sources;
	}
	inline const auto getTargets() -> std::vector<std::shared_ptr<SimpleMachineNode>> {
		return targets;
	}
	auto getBoundingBox(ImFlow::ImNodeFlow &grid) -> std::pair<ImVec2, ImVec2>;
};