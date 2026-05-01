#pragma once
#include <ImNodeFlow.h>
#include <imgui.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "Util/digraph.hpp"
#include "Nodes/simpleMachineNode.hpp"

class NodeEditor : public ImFlow::BaseNode
{
private:
    size_t idCounter = 0;
    ImFlow::ImNodeFlow grid;
    ImVec2 size;
    DiGraph digraph;
    std::pair<ImVec2, ImVec2> graphBoundingBox;
    const ImGuiWindowFlags editorFlags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;
public:
    NodeEditor(size_t gridSize);

    template <typename T>
    inline auto addNodeAtMouse() -> void
    {
        auto node = grid.placeNode<T>(getNewId());
        digraph.addNode(node->getUID());
    }

    inline auto getNewId() -> size_t {return ++idCounter;};
    inline auto setSize(ImVec2 d) -> void { grid.setSize(d); }
    auto draw() -> void override;
    inline auto printGraph() -> void { digraph.printGraph(); };
    inline auto getGrid() -> ImFlow::ImNodeFlow& {return grid;};
    inline auto getGraph() -> DiGraph& {return digraph;};
    auto getSelectedNode() -> std::shared_ptr<SimpleMachineNode>;
    auto update(ImVec2 size, std::pair<ImVec2, ImVec2> graphBoundingBox ) -> void;
    auto saveToFile(const std::string& path) -> void;
    auto loadFromAFile(const std::string& path) -> void;
    auto setNewId(size_t id) -> void {idCounter = id;};
    auto arrangeNodes() -> void;
};