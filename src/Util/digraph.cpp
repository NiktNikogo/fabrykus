#include "DiGraph.hpp"
#include <format>

auto DiGraph::getNode(const Id &id) -> GraphNode *
{
    auto it = std::find_if(nodes.begin(), nodes.end(), [&id](const auto &n)
                           { return n.data == id; });
    return it != nodes.end() ? &(*it) : nullptr;
    
}

auto DiGraph::getNode(const Id &id) const -> const GraphNode *
{
    auto it = std::find_if(nodes.begin(), nodes.end(), [&id](const auto &n)
                           { return n.data == id; });
    return it != nodes.end() ? &(*it) : nullptr;
}

auto DiGraph::addEdge(const Id &parent, const  Id&child) -> void
{
    auto* parentNode = getNode(parent);
    auto* childNode = getNode(child);
    if(parentNode && childNode) {
        parentNode->edges.push_back(std::make_shared<GraphNode>(*childNode));
    }
}

auto DiGraph::addNode(const Id &id) -> void
{
    nodes.push_back(GraphNode{id, {}});
}

auto DiGraph::removeNode(const Id &id) -> void
{
    std::erase_if(nodes, [&id](const auto& n) {
        return n.data == id;
    });
}

auto DiGraph::removeEdge(const Id &parent, const Id &id) -> void
{
    auto* node = getNode(parent);
    if(node) {
        std::erase_if( node->edges, [&id](const auto& eRef) {
            return eRef->data == id; 
        });
    }
}

auto DiGraph::hasNode(const Id &id) const -> bool {
    return getNode(id) != nullptr; 
}

auto DiGraph::hasEdge(const Id &parent, const Id &child) const -> bool{
    auto* node = getNode(parent);
    if(!node) return false;
    return std::any_of(node->edges.begin(), node->edges.end(), [&child](const auto& eRef) {
        return eRef->data == child; 
    });
}

auto DiGraph::printGraph() const -> void {
    if(nodes.empty()) {
        std::cout << "Graph is empty\n";
        return;
    }
    
    std::cout << "Graph:\n";
    for(const auto& node : nodes) {
        if(node.edges.empty()) {
            std::cout << std::format("  Node 0x{:X} -> (no edges)\n", node.data);
        } else {
            std::cout << std::format("  Node 0x{:X} -> ", node.data);
            for(size_t i = 0; i < node.edges.size(); ++i) {
                std::cout << std::format("0x{:X}", node.edges[i]->data);
                if(i < node.edges.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
    }
}

auto DiGraph::getNodes() const -> const std::vector<GraphNode> &
{
    return nodes;
}

auto DiGraph::getNodes() -> std::vector<GraphNode>&
{
   return nodes;
}

auto DiGraph::clearEdges() -> void
{
    std::for_each(nodes.begin(), nodes.end(), [](auto& n){n.edges.clear();});
}
