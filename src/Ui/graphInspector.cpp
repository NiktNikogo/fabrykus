#include "Ui/graphInspector.hpp"

#include <format>

#include "graphInspector.hpp"
#include "Nodes/simpleMachineNode.hpp"

auto GraphInspector::showCurrentGraph(ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void
{
	ImGui::SameLine();
	ImGui::Text("Inspecting Graph %d", graphIdx);
	ImGui::Separator();

	auto currGraph = graphs[graphIdx];
	auto nodes = currGraph.getNodes();

	ImGui::Spacing();
	ImGui::Text("Total nodes: ", nodes.size());

	isReverse = false; 
	ImGui::SameLine(ImGui::GetWindowWidth() - 200); 
	if (ImGui::Checkbox("Reverse Flow", &currGraph.isReversed))
	{
		currGraph.reverseFlow(grid, digraph); 
	}
	if (ImGui::CollapsingHeader("Sources", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto sources = currGraph.getSources();
		for (auto &source : sources)
		{
			ImGui::PushID(source->getId());
			size_t i = 0;
			for (auto &ing : source->getOutListRef())
			{
				ImGui::PushID(i++);
				
				const float amountWidth = 80.0f;
				ImGui::TextDisabled("Id: %d", source->getId());
				ImGui::PushItemWidth(amountWidth);

				ImGui::InputDouble(std::format("##Amt").c_str(), &ing.amount);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				
				const float nameWidth = 150.0f;
				ImGui::PushItemWidth(nameWidth);
				char buffer[SimpleMachineNode::TEXT_INPUT_MAX_LENGTH]{};
        		snprintf(buffer, sizeof(buffer), "%s", ing.name.c_str());
        		if (ImGui::InputText(std::format("##Name").c_str(), buffer, sizeof(buffer))) {
					ing.name = buffer;
				}
       
				ImGui::PopItemWidth();

				ImGui::PopID();
			}
			ImGui::PopID();
		}
	}
}

const auto GraphInspector::draw(ImFlow::ImNodeFlow &grid, DiGraph& digraph, bool canShow) -> void const
{
	if (!canShow)
		return;
	if (isHiddenByKeys)
		return;

	float screenW = ImGui::GetIO().DisplaySize.x;
	float screenH = ImGui::GetIO().DisplaySize.y;

	ImGui::SetNextWindowPos(ImVec2(screenW - inspectorWidth, menuHeight));
	ImGui::SetNextWindowSize(ImVec2(inspectorWidth, screenH - menuHeight));

	const auto flags = ImGuiWindowFlags_NoMove |
					   ImGuiWindowFlags_NoResize |
					   ImGuiWindowFlags_NoCollapse;

	if (ImGui::Begin("Graph Inspector", nullptr, flags))
	{
		ImGui::SetWindowFontScale(1.5f);
		if (graphs.empty())
		{
			ImGui::Text("No graphs found");
		}
		else if (graphIdx == -1)
		{
			isReverse = false;
			std::string preview = (graphIdx == -1) ? "Select a graph" : "Graph " + std::to_string(graphIdx);
			if (ImGui::BeginCombo("Graphs", preview.c_str()))
			{
				for (size_t i = 0; i < graphs.size(); i++)
				{
					bool isSelected = (graphIdx == i);
					std::string label = "Graph " + std::to_string(i);

					if (ImGui::Selectable(label.c_str(), isSelected))
					{
						graphIdx = i;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		if (graphIdx != -1 && graphIdx < graphs.size())
		{
			if (ImGui::Button("Go back"))
			{
				graphIdx = -1;
				ImGui::End();
				return;
			}
			showCurrentGraph(grid, digraph);
		}
	}
	ImGui::End();
}

auto GraphInspector::update(DiGraph &graph, ImFlow::ImNodeFlow &grid) -> void
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_G))
	{
		isHiddenByKeys = !isHiddenByKeys;
	}
	if (!isHiddenByKeys)
	{
		graphs = graph.getIsolatedGraphs(grid);

		hasGraphs = graphs.size() > 0;
	}
}

auto GraphInspector::setShow(bool isShowings) -> void
{
	this->hasGraphs = hasGraphs;
}

const auto GraphInspector::getShow() -> bool const
{
	return this->hasGraphs;
}

auto GraphInspector::setHiddenByKeys(bool isHidden) -> void
{
	this->isHiddenByKeys = isHidden;
}

const auto GraphInspector::getHiddenByKeys() -> bool const
{
	return this->isHiddenByKeys;
}

const auto GraphInspector::getCurrentBoundingBox(ImFlow::ImNodeFlow &grid) -> std::pair<ImVec2, ImVec2>
{
	if (graphIdx == -1)
	{
		return {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MIN}};
	}
	return graphs[graphIdx].getBoundingBox(grid);
}
