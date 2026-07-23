#include <format>
#include <ImGuiFileDialog.h>
#include <fstream>

#include "nodeEditor.hpp"
#include "Nodes/simpleMachineNode.hpp"
#include "Nodes/productNode.hpp"
#include "Nodes/ingredientNode.hpp"
#include "Internal/nodeEditorIO.hpp"
#include "Internal/imageManager.hpp"
#include "Util/nodeFactory.hpp"

NodeEditor::NodeEditor(size_t gridSize)
    : size({(float)gridSize, (float)gridSize}), grid("Editor"), digraph()
{
}

auto NodeEditor::draw() -> void
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(this->size);
    ImGui::SetNextWindowScroll(ImVec2(0.0f, 0.0f)); 
    if (ImGui::Begin("Node Editor", nullptr, editorFlags))
    {
        
        float scale = grid.getGrid().scale();
        grid.update();
        ImVec2 origin = grid.getGrid().origin();

        auto transform = [&](ImVec2 p)
        {
            return ImVec2(p.x * scale + origin.x, p.y * scale + origin.y);
        };


        ImGui::SetCursorScreenPos(ImVec2(0, 0));
        const ImGuiWindowFlags overlayFlags = ImGuiWindowFlags_NoBackground |
                                               ImGuiWindowFlags_NoInputs |
                                               ImGuiWindowFlags_NoScrollbar |
                                               ImGuiWindowFlags_NoScrollWithMouse |
                                               ImGuiWindowFlags_NoNav |
                                               ImGuiWindowFlags_NoDecoration;
        if (ImGui::BeginChild("##NodeEditorOverlay", this->size, ImGuiChildFlags_None, overlayFlags))
        {
            ImDrawList *overlayDrawList = ImGui::GetWindowDrawList();
 
            overlayDrawList->AddRect(
                graphBoundingBox.first, graphBoundingBox.second,
                IM_COL32(0, 255, 255, 255),
                10.0f,
                0,
                3.0f);
 
            if (bezierCoords.first.x != FLT_MAX)
            {
                auto p2 = bezierCoords.second;
                auto p1 = bezierCoords.first;
                float distance = sqrt(pow((p2.x - p1.x), 2.f) + pow((p2.y - p1.y), 2.f));
                float delta = distance * 0.45f;
                if (p2.x < p1.x)
                    delta += 0.2f * (p1.x - p2.x);
                float vert = 0.f;
                ImVec2 p22 = p2 - ImVec2(delta, vert);
                if (p2.x < p1.x - 50.f)
                    delta *= -1.f;
                ImVec2 p11 = p1 + ImVec2(delta, vert);
 
                static const ImU32 color = IM_COL32(255, 0, 0, 255);
                p1 = transform(p1);
                p2 = transform(p2);
                p11 = transform(p11);
                p22 = transform(p22);
                overlayDrawList->AddBezierCubic(p1, p11, p22, p2, color, 8.0f);
            }
 
            for (const auto &edge : grid.getLinks())
            {
                auto locked = edge.lock();
                auto leftPin = locked->left();
                auto rightPin = locked->right();
                auto leftNode = dynamic_cast<SimpleMachineNode *>(leftPin->getParent());
                auto rightNode = dynamic_cast<SimpleMachineNode *>(rightPin->getParent());
 
                auto pinIdx = leftNode->getOutPinIndex(leftPin);
                const auto ingredientType = leftNode->getOutList()[pinIdx].name;
 
                if (ImageManager::get().isInRegistry(ingredientType))
                {
                    auto texture = ImageManager::get().getTexture(ingredientType);
                    auto tex = texture.texID;
 
                    ImVec2 p1 = leftPin->pinPoint();
                    ImVec2 p2 = rightPin->pinPoint();
 
                    float distance = sqrt(pow((p2.x - p1.x), 2.f) + pow((p2.y - p1.y), 2.f));
                    float delta = distance * 0.45f;
                    if (p2.x < p1.x)
                        delta += 0.2f * (p1.x - p2.x);
                    float vert = 0.f;
                    ImVec2 p22 = p2 - ImVec2(delta, vert);
                    if (p2.x < p1.x - 50.f)
                        delta *= -1.f;
                    ImVec2 p11 = p1 + ImVec2(delta, vert);
 
                    auto evalBezier = [](ImVec2 cp1, ImVec2 cp11, ImVec2 cp22, ImVec2 cp2, float t)
                    {
                        float u = 1.0f - t;
                        return u * u * u * cp1 + 3.0f * u * u * t * cp11 + 3.0f * u * t * t * cp22 + t * t * t * cp2;
                    };

                    auto aspect = texture.h / texture.w;
                    static const float imageSize = 48.0f;
                    ImVec2 iconSize = {imageSize, imageSize * aspect};
                    ImVec2 center = evalBezier(p1, p11, p22, p2, 0.5f);  
                    ImVec2 topLeft = {center.x - iconSize.x / 2.0f, center.y - iconSize.y / 2.0f};
                    ImVec2 bottomRight = {center.x + iconSize.x / 2.0f, center.y + iconSize.y / 2.0f};
        
                    overlayDrawList->AddImage(
                        tex,
                        transform(topLeft),
                        transform(bottomRight),
                        ImVec2(0, 0),
                        ImVec2(1, 1),
                        IM_COL32_WHITE);
                }
            }
            ImGui::EndChild();
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
        if (ImGui::IsKeyPressed(ImGuiKey_4))
        {
            auto links = grid.getLinks();
            for (auto link : links)
            {
                auto locked = link.lock();
                std::cout << locked->right() << " " << locked->left() << '\n';
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_5))
        {
            auto iso = digraph.getIsolatedGraphs(grid);
            std::cout << iso.size() << '\n';
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
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_A))
        {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("AddProjectKey",
                                                    "Add project", ".json", config);
        }
 
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_D))
        {
            arrangeNodes(LayoutStyle::SORT);
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F))
        {
            arrangeNodes(LayoutStyle::ORGANIZE);
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
                    arrangeNodes(LayoutStyle::SORT);
                }
                if (ImGui::MenuItem("Organize nodes", "Ctrl+F"))
                {
                    arrangeNodes(LayoutStyle::ORGANIZE);
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

auto NodeEditor::update(ImVec2 size, std::pair<ImVec2, ImVec2> graphBoundingBox, std::pair<ImVec2, ImVec2> bezierCoords) -> void
{
    this->setSize(size);
    this->size = size;

    if (ImGuiFileDialog::Instance()->IsOpened())
    {
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
            digraph.clearEdges();
            digraph.clearNodes();
            std::vector<DiGraph::Id> ids;
            for (const auto &node : grid.getNodes())
            {
                ids.push_back(node.second->getUID());
            }
            for (auto id : ids)
            {
                grid.getNodes().at(id)->destroy();
            }
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            loadFromAFile(filePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }
    if (ImGuiFileDialog::Instance()->Display("AddProjectKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            loadFromAFile(filePath);
        }
        ImGuiFileDialog::Instance()->Close();
    }

    typedef ImFlow::NodeUID NodeId;
    std::set<NodeId> aliveIds;
    for (const auto &node : grid.getNodes())
    {
        aliveIds.insert(node.second->getUID());
    }
    for (const auto &dataId : digraph.getNodes())
    {
        if (aliveIds.find(dataId) == aliveIds.end())
        {
            digraph.removeNodeEdges(dataId);
            digraph.removeNode(dataId);
        }
    }

    digraph.clearEdges();
    for (const auto &link : grid.getLinks())
    {
        auto locked = link.lock();
        if (locked == nullptr)
        {
            continue;
        }

        auto leftPin = locked->left();
        auto leftNode = leftPin->getParent();
        auto left = leftNode->getUID();
        auto pinY = locked->left()->getPos().y - leftNode->getPos().y;
        auto right = locked->right()->getParent()->getUID();
        if (!digraph.hasEdge(left, right))
        {
            digraph.addEdge(left, right, pinY);
        }
    }
    this->bezierCoords = bezierCoords;
    this->graphBoundingBox = graphBoundingBox;
}

auto NodeEditor::saveToFile(const std::string &path) -> void
{
    NodeEditorIO::save(path, grid);
}

auto NodeEditor::loadFromAFile(const std::string &path) -> void
{
    idCounter = NodeEditorIO::load(path, grid, digraph, idCounter);
}

auto NodeEditor::arrangeNodes(LayoutStyle style) -> void
{
    digraph.clearEdges();
    for (const auto &link : grid.getLinks())
    {
        auto locked = link.lock();
        auto leftPin = locked->left();
        auto leftNode = leftPin->getParent();
        auto left = leftNode->getUID();
        auto pinY = locked->left()->getPos().y - leftNode->getPos().y;
        auto right = locked->right()->getParent()->getUID();
        if (!digraph.hasEdge(left, right))
        {
            digraph.addEdge(left, right, pinY);
        }
    }

    std::map<ImFlow::NodeUID, ImVec2> nodeSizes{};
    std::map<ImFlow::NodeUID, ImVec2> nodePositions{};
    for (const auto &[id, node] : grid.getNodes())
    {
        nodeSizes[id] = node->getSize();
        nodePositions[id] = node->getPos();
    }
    std::optional<std::map<DiGraph::Id, ImVec2>> newPositionsOpt{};
    if (style == LayoutStyle::SORT)
    {
        newPositionsOpt = digraph.calculateShiftedPositions(nodeSizes, nodePositions);
    }
    else if (style == LayoutStyle::ORGANIZE)
    {
        newPositionsOpt = digraph.calculateStacked(nodeSizes);
    }
    if (!newPositionsOpt.has_value())
        return;

    auto newPositions = *newPositionsOpt;
    for (auto &[id, node] : grid.getNodes())
    {
        auto newPos = newPositions[id];
        node->setPos(newPos);
    }
}
