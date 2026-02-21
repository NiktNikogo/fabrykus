#include <format>
#include <ImGuiFileDialog.h>
#include <fstream>

#include "nodeEditor.hpp"
#include "Nodes/simpleMahcineNode.hpp"
#include "Nodes/productNode.hpp"
#include "Nodes/ingredientNode.hpp"

NodeEditor::NodeEditor(size_t gridSize)
    : size({(float)gridSize, (float)gridSize}), grid("Editor"), digraph()
{
}

auto NodeEditor::draw() -> void
{
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
                    this->addNodeAtMouse<IngredientNode>();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Utils"))
            {

                if (ImGui::MenuItem("Sort nodes", "Ctrl+D"))
                {
                    std::cout << "sort pog?\n";
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

auto NodeEditor::collectData() -> nlohmann::json
{
    nlohmann::json production;
    production["nodes"] = nlohmann::json::array();
    auto nodes = grid.getNodes();
    if (nodes.size() < 1)
        return production;
    for (const auto &[id, node] : nodes)
    {
        auto myNode = std::dynamic_pointer_cast<SimpleMachineNode>(node);
        production["nodes"].push_back(myNode->serialize());
    }
    production["links"] = serializeLinks();
    return production;
}

auto NodeEditor::parseNodes(nlohmann::json nodes) -> void
{
    size_t biggestId = 0;
    for (auto data : nodes)
    {
        size_t id = data["id"];
        biggestId = biggestId > id ? biggestId : id;
        NodeType type = data["type"].get<NodeType>();
        switch (type)
        {
        case NodeType::MACHINE:
        {            
            auto node = grid.placeNode<SimpleMachineNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        case NodeType::INGREDIENT:
        {
     
            auto node = grid.placeNode<IngredientNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        case NodeType::PRODUCT:
        {
            auto node = grid.placeNode<ProductNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        default:
            std::cout << "Bad data\n";
            break;
        }
    }
    idCounter = biggestId;
}

auto NodeEditor::parseLinks(nlohmann::json links) -> void
{
    auto nodes = grid.getNodes();
    std::unordered_map<int, std::shared_ptr<SimpleMachineNode>> nodeMap;
    for(const auto [id, node] : nodes) {
        auto casted = std::dynamic_pointer_cast<SimpleMachineNode>(node);
        nodeMap[casted->getId()] =  casted;
    }

    for(auto data : links) {
        size_t fromNodeId = data["from_node_id"];
        size_t fromPinIdx = data["from_pin_idx"];
        size_t toNodeId = data["to_node_id"];
        size_t toPinIdx = data["to_pin_idx"];

        auto fromNode = nodeMap[fromNodeId];
        auto fromPin = fromNode->getOutListElement(fromPinIdx);
        auto toNode = nodeMap[toNodeId];
        auto toPin = toNode->getInListElement(toPinIdx);
        

        auto link = std::make_shared<ImFlow::Link>(fromPin, toPin, &grid);
        
        fromPin->createLink(toPin);
    }
  
}

auto NodeEditor::saveToFile(const std::string &path) -> void
{
    auto production = collectData();
    std::ofstream file(path);
    if (file.is_open())
    {
        file << production.dump(4);
        file.close();
    }
}

auto NodeEditor::loadFromAFile(const std::string &path) -> void
{
    std::ifstream file(path);
    nlohmann::json production = nlohmann::json::parse(file);
    parseNodes(production["nodes"]);
    parseLinks(production["links"]);
}

auto NodeEditor::serializeLinks() -> nlohmann::json
{
    nlohmann::json jsonLinks = nlohmann::json::array();

    auto links = grid.getLinks();
    if (links.size() < 1)
        return jsonLinks;
    for (const auto &link : links)
    {
        auto locked = link.lock();
        auto right = locked->right(), left = locked->left();
        auto rightParent = dynamic_cast<SimpleMachineNode *>(right->getParent());
        auto leftParent = dynamic_cast<SimpleMachineNode *>(left->getParent());

        auto fromPinIdx = leftParent->getOutPinIndex(left);
        auto fromNodeId = leftParent->getId();
        auto toPinIdx = rightParent->getInPinIndex(right);
        auto toNodeId = rightParent->getId();
        jsonLinks.push_back({
            {"from_pin_idx", fromPinIdx},
            {"from_node_id", fromNodeId},
            {"to_pin_idx", toPinIdx},
            {"to_node_id", toNodeId},
        });
    }
    return jsonLinks;
}
