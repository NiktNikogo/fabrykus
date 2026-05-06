#pragma once

#include <vector>
#include <memory>
#include <imgui.h>
#include "ImNodeFlow.h"
#include "Nodes/simpleMachineNode.hpp"

class DiGraph;

class IsolatedGraph {
private:
	
	using Id = ImFlow::NodeUID;
	using NodesCollection = std::vector<std::shared_ptr<SimpleMachineNode>>;
	std::vector<Id> ids;
	NodesCollection sources;
	NodesCollection nodes;
	NodesCollection targets;
	NodesCollection machines;
public:
	bool isReversed = false;
private:
	auto arrangeNodes(ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void;
public:
	IsolatedGraph();
	IsolatedGraph(std::vector<Id> ids);
	auto groupNodes(ImFlow::ImNodeFlow &grid) -> void;
	auto isProductionLine() const -> bool;
	inline const auto getNodes() -> NodesCollection {
		return nodes;
	}
	inline const auto getSources() -> NodesCollection {
		return sources;
	}
	inline const auto getTargets() -> NodesCollection {
		return targets;
	}
	inline const auto getMachines() -> NodesCollection {
		return machines;
	}
	auto getBoundingBox(ImFlow::ImNodeFlow &grid) -> std::pair<ImVec2, ImVec2>;
	auto reverseFlow(ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void;
	inline void setIsReversed(bool isReversed) {
		this->isReversed = isReversed;
	}
};