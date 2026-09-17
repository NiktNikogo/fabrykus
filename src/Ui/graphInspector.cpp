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

		if (ImGui::BeginTable("##inputSummaryTable", 3, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Id");
			ImGui::TableSetupColumn("Amount");
			ImGui::TableSetupColumn("Name");
			ImGui::TableHeadersRow();

			for (auto &source : sources)
			{
				ImGui::PushID(source->getId());
				size_t i = 0;
				for (auto &ing : source->getOutList())
				{
					inputMap[ing.name] += ing.amount;

					ImGui::PushID(i++);
					ImGui::TableNextRow();

					ImGui::TableNextColumn();
					ImGui::Text("%zu", source->getId());

					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					double tempVal = ing.asDouble();
					if (ImGui::InputDouble("##Amt", &tempVal))
					{
						ing.fromDouble(tempVal);
					}

					ImGui::TableNextColumn();
					ImGui::SetNextItemWidth(-FLT_MIN);
					char buffer[SimpleMachineNode::TEXT_INPUT_MAX_LENGTH]{};
					snprintf(buffer, sizeof(buffer), "%s", ing.name.c_str());
					if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
					{
						ing.name = buffer;
					}

					ImGui::PopID();
				}
				ImGui::PopID();
			}

			ImGui::EndTable();
		}
	}
	if (ImGui::CollapsingHeader("Machines", ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto machines = currGraph.getMachines();

		if (ImGui::BeginTable("##machinesTable", 3, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Machine");
			ImGui::TableSetupColumn("Used");
			ImGui::TableSetupColumn("Bottleneck");
			ImGui::TableHeadersRow();

			size_t i = 0;
			for (const auto &machine : machines)
			{
				auto count = boost::rational_cast<size_t>(machine->calcOptimalCount());
				auto bottleneck = boost::rational_cast<double>(machine->calcBottleneck());

				ImGui::PushID(i++);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::Text("Machine %zu", machine->getId());

				ImGui::TableNextColumn();
				ImGui::Text("%zu", count);

				ImGui::TableNextColumn();
				ImVec4 color;
				if (bottleneck > 0.75f)
					color = { 0.0f, 1.0f, 0.0f, 1.0f };
				else if (bottleneck > 0.5f)
					color = { 1.0f, 0.3f, 0.3f, 1.0f };
				else
					color = { 1.0f, 0.0f, 0.0f, 1.0f };

				ImGui::TextColored(color, "%.2f%%", bottleneck * 100);

				ImGui::PopID();
			}

			ImGui::EndTable();
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
		if (ImGui::BeginTable("##producingTable", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Ingredient");
			ImGui::TableSetupColumn("Rate");
			ImGui::TableHeadersRow();

			for (const auto &[name, amount] : outputMap)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(name.c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%.2f u/s", boost::rational_cast<double>(amount));
			}

			ImGui::EndTable();
		}

		ImGui::Text("Relationships: ");
		auto ratios = currGraph.getOutputRatios(grid, digraph);
		for (const auto &[outName, outAmount] : outputMap)
		{
			if (ImGui::TreeNode(outName.c_str()))
			{
				auto it = std::find_if(ratios.begin(), ratios.end(), [&](const auto &pair) {
					return pair.first.name == outName;
				});

				if (it == ratios.end() || it->second.empty())
				{
					ImGui::TextDisabled("Nothing contributes");
				}
				else
				{
					double out = boost::rational_cast<double>(outAmount);

					if (ImGui::BeginTable("##ratioTable", 2, ImGuiTableFlags_SizingStretchProp))
					{
						ImGui::TableSetupColumn("Ingredient");
						ImGui::TableSetupColumn("Amount");
						ImGui::TableHeadersRow();

						for (const auto &ing : it->second)
						{
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::Text("%s : %s", outName.c_str(), ing.name.c_str());
							ImGui::TableNextColumn();
							ImGui::Text("%.2f : %.2f", out, ing.asFloat());
						}

						ImGui::EndTable();
					}
				}

				ImGui::TreePop();
			}
		}
	}
}

const auto GraphInspector::draw(ImFlow::ImNodeFlow &grid, DiGraph &digraph, bool canShow) -> void const
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
	auto tempGraphs = graph.getIsolatedGraphs(grid);
	if (tempGraphs.size() != graphs.size())
	{
		graphIdx = -1;
	}
	graphs = tempGraphs;
	hasGraphs = graphs.size() > 0;
	
}

auto GraphInspector::setShow(bool isShowings) -> void
{
	this->hasGraphs = isShowings;
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
