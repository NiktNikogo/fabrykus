#include "nodeEditor.hpp"

NodeEditor::NodeEditor(size_t gridSize)
    : size({(float)gridSize, (float)gridSize}), grid("Editor"), digraph()
{
}

auto NodeEditor::addNodeAtMouse() -> void
{
    auto node = grid.placeNode<SimpleNode<2>>();
    digraph.addNode(node->getUID());
}

auto NodeEditor::draw() -> void
{
    grid.update();
    if (ImGui::IsKeyPressed(ImGuiKey_K))
    {
        digraph.clearEdges();
        for (const auto &link : grid.getLinks())
        {
            auto locked = link.lock();
            auto left = locked->left()->getParent()->getUID();
            auto right = locked->right()->getParent()->getUID();
            std::cout << std::format("Left: 0x{:X} Right: 0x{:X}", left, right) << '\n';
            if (!digraph.hasEdge(left, right)) {
                digraph.addEdge(left, right);
            }
        }
    }
}
