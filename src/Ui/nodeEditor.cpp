#include <format>

#include "nodeEditor.hpp"
#include "Nodes/simpleMahcineNode.hpp"
#include "Nodes/productNode.hpp"
#include "Nodes/ingridientNode.hpp"
NodeEditor::NodeEditor(size_t gridSize)
    : size({(float)gridSize, (float)gridSize}), grid("Editor"), digraph()
{
}

auto NodeEditor::draw() -> void
{
    if (ImGui::Begin("Node Editor", nullptr, editorFlags))
    {
        grid.update();
        if (ImGui::IsKeyPressed(ImGuiKey_1))
        {
            digraph.clearEdges();
            for (const auto &link : grid.getLinks())
            {
                auto locked = link.lock();
                auto left = locked->left()->getParent()->getUID();
                auto right = locked->right()->getParent()->getUID();
                std::cout << std::format("Left: 0x{:X} Right: 0x{:X}", left, right) << '\n';
                if (!digraph.hasEdge(left, right))
                {
                    digraph.addEdge(left, right);
                }
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_2))
        {
            digraph.printTopologicalSort();
        }
        if (ImGui::IsKeyPressed(ImGuiKey_3))
        {
            auto depths = digraph.calcNodeDepths();
            if (depths.has_value())
            {
                for (const auto &[nodeId, depth] : *depths)
                {
                    std::cout << std::format("Node 0x{:X} is at depth {}\n", nodeId, depth);
                }
            }

            digraph.printByDepth();
        }

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("NodeEditorContext");
        }

        if (ImGui::BeginPopup("NodeEditorContext"))
        {
            if (ImGui::BeginMenu("Machines"))
            {
                if (ImGui::MenuItem("Machine"))
                {
                    this->addNodeAtMouse<SimpleMachineNode>();
                }
                if (ImGui::MenuItem("Storage"))
                {
                    this->addNodeAtMouse<ProductNode>();
                }
                if (ImGui::MenuItem("Source"))
                {
                    this->addNodeAtMouse<IngridientNode>();
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        ImGui::End();
    }
}

auto NodeEditor::getSelectedNode() -> std::shared_ptr<SimpleMachineNode>
{
    for (auto &pair : grid.getNodes())
    {
        auto node = pair.second;

        if (node->isSelected())
        {
            return std::dynamic_pointer_cast<SimpleMachineNode>(node);
        }
    }
    return nullptr;
}

auto NodeEditor::update(ImVec2 size) -> void
{
    this->setSize(size);
}
