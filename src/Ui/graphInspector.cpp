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

	std::map<std::string, Rational> inputMap;

	ImGui::Spacing();
	ImGui::Text("Total nodes %zu: ", nodes.size());
	if (nodes.size() > 0)
	{
		currGraph.setIsReversed(nodes[0]->getIsReversed());
	}
	ImGui::SameLine(ImGui::GetWindowWidth() - 200);
	if (ImGui::Checkbox("Reverse Flow", &currGraph.isReversed))
	{
		currGraph.reverseFlow(grid, digraph);
	}
	if (ImGui::CollapsingHeader("Input summary", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto sources = currGraph.getSources();
		for (auto &source : sources)
		{
			ImGui::PushID(source->getId());
			size_t i = 0;
			for (auto &ing : source->getOutList())
			{
				inputMap[ing.name] += ing.amount;

				ImGui::PushID(i++);

				const float amountWidth = 80.0f;
				ImGui::Text("Id: %zu", source->getId());
				ImGui::PushItemWidth(amountWidth);

				double tempVal = ing.asDouble();
				if (ImGui::InputDouble(std::format("##Amt").c_str(), &tempVal))
				{
					ing.fromDouble(tempVal);
				}
				ImGui::PopItemWidth();
				ImGui::SameLine();

				const float nameWidth = 150.0f;
				ImGui::PushItemWidth(nameWidth);
				char buffer[SimpleMachineNode::TEXT_INPUT_MAX_LENGTH]{};
				snprintf(buffer, sizeof(buffer), "%s", ing.name.c_str());
				if (ImGui::InputText(std::format("##Name").c_str(), buffer, sizeof(buffer)))
				{
					ing.name = buffer;
				}

				ImGui::PopItemWidth();

				ImGui::PopID();
			}
			ImGui::PopID();
		}
	}
	if (ImGui::CollapsingHeader("Machines", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto machines = currGraph.getMachines();
		size_t i = 0;
		for (const auto &machine : machines)
		{
			auto count = boost::rational_cast<size_t>(machine->calcOptimalCount());
			auto bottleneck = boost::rational_cast<double>(machine->calcBottleneck());
			ImGui::PushID(i++);
			ImGui::Text("Machine %zu: used: %zu |", machine->getId(), count);
			ImGui::SameLine();

			ImVec4 color;
			if (bottleneck > 0.75f)
			{
				color = {0.0f, 1.0f, 0.0f, 1.0f};
			}
			else if (bottleneck < 0.75f && bottleneck > 0.5f)
			{
				color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
			}
			else
			{
				color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
			}
			ImGui::TextColored(color, "bottleneck: %.2f", bottleneck * 100);
			ImGui::PopID();
		}
	}
	if (ImGui::CollapsingHeader("Output summary", ImGuiTreeNodeFlags_DefaultOpen))
	{
		std::map<std::string, Rational> outputMap;
		for (const auto &target : currGraph.getTargets())
		{
			for (const auto &ing : target->getInList())
			{
				outputMap[ing.name] += ing.amount;
			}
		}
		ImGui::Text("Producing: ");
		for (const auto &[name, amount] : outputMap)
		{
			ImGui::Text("%s: %.2f u/s", name.c_str(), boost::rational_cast<double>(amount));
		}
		ImGui::Text("Relationships: ");
		for (const auto &[outName, outAmount] : outputMap)
		{
			for (const auto &[inName, inAmount] : inputMap)
			{
				if (ImGui::TreeNode(outName.c_str()))
				{
					for (const auto &[inName, inAmount] : inputMap)
					{
						auto ratio = Rational(0);
						if (inAmount != Rational(0))
						{
							ratio = outAmount / inAmount;
						}
						ImGui::Text("Ratio %lld : %lld | %s:%s", ratio.numerator(), ratio.denominator(), outName.c_str(), inName.c_str());
					}
					ImGui::TreePop();
				}
			}
		}
	}
}

const auto GraphInspector::draw(ImFlow::ImNodeFlow &grid, DiGraph &digraph, bool canShow) -> void const
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
		auto tempGraphs = graph.getIsolatedGraphs(grid);
		if (tempGraphs.size() != graphs.size())
		{
			graphIdx = -1;
		}
		graphs = tempGraphs;
		hasGraphs = graphs.size() > 0;
	}
}

auto GraphInspector::setShow(bool isShowings) -> void
{
	this->hasGraphs = hasGraphs;
}

const auto GraphInspector::getShow() -> bool const
{
	return hasGraphs;
}

auto GraphInspector::setHiddenByKeys(bool isHidden) -> void
{
	this->isHiddenByKeys = isHidden;
}

const auto GraphInspector::getHiddenByKeys() -> bool const
{
	return isHiddenByKeys;
}

const auto GraphInspector::getCurrentBoundingBox(ImFlow::ImNodeFlow &grid) -> std::pair<ImVec2, ImVec2>
{
	if (graphIdx == -1)
	{
		return {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MIN}};
	}
	return graphs[graphIdx].getBoundingBox(grid);
}
