#include "edgeInspector.hpp"

const auto EdgeInspector::draw(ImFlow::ImNodeFlow &grid, bool canShow) -> void const
{
	// if (!canShow)
	// 	return;
	if (isHiddenByKeys)
		return;

	float screenW = ImGui::GetIO().DisplaySize.x;
	float screenH = ImGui::GetIO().DisplaySize.y;

	ImGui::SetNextWindowPos(ImVec2(screenW - inspectorWidth, menuHeight));
	ImGui::SetNextWindowSize(ImVec2(inspectorWidth, screenH - menuHeight));

	const auto flags = ImGuiWindowFlags_NoMove |
					   ImGuiWindowFlags_NoResize |
					   ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("Edge Inspector", nullptr, flags))
	{
		ImGui::SetWindowFontScale(1.5f);
		if (!selectedEdge)
		{
			ImGui::Text("No edge found");
		}
		else
		{
			auto leftPin = selectedEdge->left();
			auto rightPin = selectedEdge->right();
			auto leftParent = leftPin->getParent();
			auto rightParent = rightPin->getParent();

			ImGui::Text("Selected edge: %p", selectedEdge);
			ImGui::Separator();

			if (leftParent && rightParent)
			{
				ImGui::Text("From Node ID: %d", leftParent->getUID());
				ImGui::Text("To Node Id: %d", rightParent->getUID());
			}
			if (ImGui::Button("Deselect"))
			{
				selectedEdge = nullptr;
				ImGui::End();
				return;
			}

		}
	}

	ImGui::End();
}

const auto EdgeInspector::update(DiGraph &graph, ImFlow::ImNodeFlow &grid) -> void
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_T))
	{
		isHiddenByKeys = !isHiddenByKeys;
	}
	for (const auto link : grid.getLinks())
	{
		auto locked = link.lock().get();
		if (locked->isSelected())
		{
			selectedEdge = locked;
		}
	}
	hasEdge = selectedEdge ? true : false;
}

auto EdgeInspector::setShow(bool isShowing) -> void
{
	hasEdge = isShowing;
}

const auto EdgeInspector::getShow() -> bool const
{
	return hasEdge;
}

auto EdgeInspector::setHiddenByKeys(bool isHidden) -> void
{
	isHiddenByKeys = isHidden;
}

const auto EdgeInspector::getHiddenByKeys() -> bool const
{
	return isHiddenByKeys;
}

const auto EdgeInspector::getBezierCoords(ImFlow::ImNodeFlow& grid) -> std::pair<ImVec2, ImVec2> const
{
	if(selectedEdge) {
		if(selectedEdge->left() && selectedEdge->right()) {
			auto leftPos = selectedEdge->left()->pinPoint();
			auto rightPos = selectedEdge->right()->pinPoint();
			
			return {leftPos, rightPos};
		} else {
			return {{FLT_MAX, FLT_MAX}, {FLT_MAX, FLT_MAX}};
		}
	} else {
		return {{FLT_MAX, FLT_MAX}, {FLT_MAX, FLT_MAX}};
	}
}
