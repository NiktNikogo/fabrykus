#include "nodeEditor.hpp"
#include "Nodes/simpleMahcineNode.hpp"
NodeEditor::NodeEditor(size_t gridSize)
    : size({(float)gridSize, (float)gridSize}), grid("Editor"), digraph()
{
}

auto NodeEditor::draw() -> void
{
    grid.update();
    // if (ImGui::IsKeyPressed(ImGuiKey_K))
    // {
    //     digraph.clearEdges();
    //     for (const auto &link : grid.getLinks())
    //     {
    //         auto locked = link.lock();
    //         auto left = locked->left()->getParent()->getUID();
    //         auto right = locked->right()->getParent()->getUID();
    //         std::cout << std::format("Left: 0x{:X} Right: 0x{:X}", left, right) << '\n';
    //         if (!digraph.hasEdge(left, right)) {
    //             digraph.addEdge(left, right);
    //         }
    //     }
    // }
    // if (ImGui::IsKeyPressed(ImGuiKey_T)) {
    //     digraph.printTopologicalSort();
    // }

    // if(ImGui::IsKeyPressed(ImGuiKey_D)) {
    //     auto depths = digraph.calcNodeDepths();
    //     if (depths.has_value()) {
    //         for (const auto& [nodeId, depth] : *depths) {
    //             std::cout << std::format("Node 0x{:X} is at depth {}\n", nodeId, depth);
    //         }
    //     }

    //     digraph.printByDepth();
    // }
}
