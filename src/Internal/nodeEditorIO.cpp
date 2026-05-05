#include "nodeEditorIO.hpp"

#include <fstream>

#include "Nodes/simpleMachineNode.hpp"
#include "Nodes/ingredientNode.hpp"
#include "Nodes/productNode.hpp"
#include "Util/nodeFactory.hpp"


auto NodeEditorIO::serializeLinks(ImFlow::ImNodeFlow &grid) -> nlohmann::json
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

auto NodeEditorIO::collectData(ImFlow::ImNodeFlow &grid) -> nlohmann::json
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
    production["links"] = NodeEditorIO::serializeLinks(grid);
    return production;
}

auto NodeEditorIO::parseNodes(ImFlow::ImNodeFlow &grid, nlohmann::json nodes, DiGraph& digraph, size_t maxCurrId) -> size_t
{
    size_t biggestId = 0;
    for (auto data : nodes)
    {
        size_t id = data["id"];
        id += maxCurrId;
        data["id"] = id;
        biggestId = biggestId > id ? biggestId : id;
        NodeType type = data["type"].get<NodeType>();
        switch (type)
        {
        case NodeType::MACHINE:
        case NodeType::INGREDIENT:
        case NodeType::PRODUCT:
        case NodeType::SPLITTER:
        case NodeType::MERGER:
            NodeFactory::createNode(type, data, grid, digraph);
        break;
        default:
            std::cout << "Bad data\n";
            break;
        }
    }
    return biggestId;
}

auto NodeEditorIO::parseLinks(ImFlow::ImNodeFlow &grid, nlohmann::json links, DiGraph& digraph, size_t maxCurrId) -> void
{
    auto nodes = grid.getNodes();
    std::unordered_map<int, std::shared_ptr<SimpleMachineNode>> nodeMap;
    for(const auto [id, node] : nodes) {
        auto casted = std::dynamic_pointer_cast<SimpleMachineNode>(node);
        nodeMap[casted->getId()] =  casted;
    }

    for(auto data : links) {
        size_t fromNodeId = data["from_node_id"];
        fromNodeId += maxCurrId;
        size_t fromPinIdx = data["from_pin_idx"];
        size_t toNodeId = data["to_node_id"];
        toNodeId += maxCurrId;
        size_t toPinIdx = data["to_pin_idx"];

        auto fromNode = nodeMap[fromNodeId];
        auto fromPin = fromNode->getOutListElement(fromPinIdx);
        auto toNode = nodeMap[toNodeId];
        auto toPin = toNode->getInListElement(toPinIdx);
        digraph.addEdge(fromNode->getUID(), toNode->getUID(),fromNode->getPos().y);

        auto link = std::make_shared<ImFlow::Link>(fromPin, toPin, &grid);
        
        fromPin->createLink(toPin);
    }
}

auto NodeEditorIO::save(const std::string &path, ImFlow::ImNodeFlow &grid) -> void
{
    auto production = collectData(grid);
    std::ofstream file(path);
    if (file.is_open())
    {
        file << production.dump(4);
        file.close();
    }
}

auto NodeEditorIO::load(const std::string &path, ImFlow::ImNodeFlow &grid, DiGraph &digraph, size_t maxCurrId) -> size_t
{
    std::ifstream file(path);
    nlohmann::json production = nlohmann::json::parse(file);
    size_t newMaxId = parseNodes(grid, production["nodes"], digraph, maxCurrId);
    parseLinks(grid, production["links"], digraph, maxCurrId);
    return newMaxId;
}
