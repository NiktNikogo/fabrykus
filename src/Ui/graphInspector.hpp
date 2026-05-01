#pragma once

#include <imgui.h>
#include <ImNodeFlow.h>
#include "Util/isolatedGraph.hpp"
#include "Util/digraph.hpp"

class GraphInspector {
private:
	bool hasGraphs = false;
	bool isHiddenByKeys;
	std::vector<IsolatedGraph> graphs;
	int graphIdx = -1;
public:
	const float inspectorWidth = 500.0f;
	const float menuHeight = 25.0f;

private:
	auto showCurrentGraph(ImFlow::ImNodeFlow &grid) -> void;

public:
	GraphInspector() 
		:
		hasGraphs(true),
		isHiddenByKeys(true),
		graphs()
	{}

	const auto draw(ImFlow::ImNodeFlow& grid, bool canShow) -> void const;
	auto update(DiGraph& graph, ImFlow::ImNodeFlow& grid) -> void;
	auto setShow(bool isShowing) -> void;
	const auto getShow() -> bool const;
	auto setHiddenByKeys(bool isHidden) -> void;
	const auto getHiddenByKeys() -> bool const;
	const auto getCurrentBoundingBox(ImFlow::ImNodeFlow &grid) -> std::pair<ImVec2, ImVec2>;
};

