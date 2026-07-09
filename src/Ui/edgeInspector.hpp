#pragma once

#include <imgui.h>
#include <ImNodeFlow.h>
#include "Util/digraph.hpp"

class EdgeInspector {
private:
	bool hasEdge = false;
	bool isHiddenByKeys;
	ImFlow::Link* selectedEdge;
public:
	const float inspectorWidth = 500.0f;
	const float menuHeight = 25.0f;

public:
	EdgeInspector()
	:
	hasEdge(false),
	isHiddenByKeys(true),
	selectedEdge(nullptr)
	{}

	const auto draw(ImFlow::ImNodeFlow& grid, bool canShow) -> void const;
	const auto update(DiGraph& graph, ImFlow::ImNodeFlow& grid) -> void;
	auto setShow(bool isShowing) -> void;
	const auto getShow() -> bool const;
	auto setHiddenByKeys(bool isHidden) -> void;
	const auto getHiddenByKeys() -> bool const;
	const auto getBezierCoords(ImFlow::ImNodeFlow& grid) -> std::pair<ImVec2, ImVec2> const;
};
