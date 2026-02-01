#pragma once
#include <ImNodeFlow.h>
#include <imgui.h>
#include <iostream>

#include "Util/digraph.hpp"
#include "Nodes/simpleMahcineNode.hpp"

class NodeEditor : public ImFlow::BaseNode
{
private:
    ImFlow::ImNodeFlow grid;
    ImVec2 size;
    DiGraph digraph;

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
        auto node = grid.placeNode<T>();
        digraph.addNode(node->getUID());
    }

    auto addEdge(const ImVec2 &pos) -> void;

    inline auto setSize(ImVec2 d) -> void { grid.setSize(d); }
    auto draw() -> void override;
    inline auto printGraph() const -> void { digraph.printGraph(); };
    auto getSelectedNode() -> std::shared_ptr<SimpleMachineNode>;
    auto update(ImVec2 size) -> void;
};