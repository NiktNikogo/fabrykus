#include <format>
#include <ImGuiFileDialog.h>
#include <fstream>

#include "nodeEditor.hpp"
#include "Nodes/simpleMachineNode.hpp"
#include "Nodes/productNode.hpp"
#include "Nodes/ingredientNode.hpp"
#include "Internal/nodeEditorIO.hpp"
#include "Util/nodeFactory.hpp"

NodeEditor::NodeEditor(size_t gridSize)
    : size({(float)gridSize, (float)gridSize}), grid("Editor"), digraph()
{
}

auto NodeEditor::draw() -> void
{   
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(this->size);
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

        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !ImGuiFileDialog::Instance()->IsOpened())
        {
            ImGui::OpenPopup("NodeEditorContext");
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("SaveProjectKey",
                                                    "Save project", ".json", config);
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_O))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("OpenProjectKey",
                                                    "Open project", ".json", config);
        }

        if (ImGui::BeginPopup("NodeEditorContext"))
        {

            if (ImGui::BeginMenu("Machines"))
            {
                for (auto const &item : NodeFactory::typeNameList)
                {
                    if (ImGui::MenuItem(item.name.c_str()))
                    {
                        NodeFactory::addNode(item.type, getNewId(), grid, digraph);
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Utils"))
            {

                if (ImGui::MenuItem("Sort nodes", "Ctrl+D"))
                {
                    arrangeNodes();
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
    this->size = size;

    if (ImGuiFileDialog::Instance()->IsOpened()) {
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowSize(ImVec2(screenSize.x * 0.8f, screenSize.y * 0.8f), ImGuiCond_Appearing);
        ImGui::SetNextWindowPos(ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    if (ImGuiFileDialog::Instance()->Display("SaveProjectKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            saveToFile(filePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("OpenProjectKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            loadFromAFile(filePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

auto NodeEditor::saveToFile(const std::string &path) -> void
{
    NodeEditorIO::save(path, grid);
}

auto NodeEditor::loadFromAFile(const std::string &path) -> void
{
    idCounter = NodeEditorIO::load(path, grid, digraph);
}

auto NodeEditor::arrangeNodes() -> void
{
    digraph.clearEdges();
    for (const auto &link : grid.getLinks())
    {
        auto locked = link.lock();
        auto left = locked->left()->getParent()->getUID();
        auto right = locked->right()->getParent()->getUID();
        if (!digraph.hasEdge(left, right))
        {
            digraph.addEdge(left, right);
        }
    }

    std::map<ImFlow::NodeUID, ImVec2> nodeSizes{};
    for(const auto& [id, node] : grid.getNodes()) {
      nodeSizes[id] = node->getSize();
    }

    auto newPositionsOpt = digraph.calculatePositions(nodeSizes);

    if(!newPositionsOpt.has_value()) return;

    auto newPositions = *newPositionsOpt;
    auto mousePos = grid.screen2grid(ImGui::GetMousePos());
    for(auto& [id, node] : grid.getNodes()) {
        auto newPos = newPositions[id] + ImVec2{mousePos.x, mousePos.y};
        node->setPos(newPos);
    }
}
