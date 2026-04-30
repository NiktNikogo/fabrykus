#pragma once

#include <vector>
#include <memory>
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
};