#pragma once
#include <ImNodeFlow.h>
#include <imgui.h>
#include <iostream>

#include "Util/digraph.hpp"

class NodeEditor : public ImFlow::BaseNode
{
private:
    ImFlow::ImNodeFlow grid;
    ImVec2 size;
    DiGraph digraph;

public:
    NodeEditor(size_t gridSize);

    template <typename T>
    auto addNodeAtMouse() -> void
    {
        auto node = grid.placeNode<T>();
        digraph.addNode(node->getUID());
    }

    auto addEdge(const ImVec2 &pos) -> void;

    inline auto set_size(ImVec2 d) -> void { grid.setSize(d); }
    auto draw() -> void override;
    inline auto printGraph() const -> void { digraph.printGraph(); };
};