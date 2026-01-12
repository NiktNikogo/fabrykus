#include "digraph.hpp"
#include <format>
#include <unordered_map>
#include <queue>

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

auto DiGraph::addEdge(const Id &parent, const Id &child) -> void
{
    auto *parentNode = getNode(parent);
    auto *childNode = getNode(child);
    if (parentNode && childNode)
    {
        parentNode->edges.push_back(std::make_shared<GraphNode>(*childNode));
    }
}

auto DiGraph::addNode(const Id &id) -> void
{
    nodes.push_back(GraphNode{id, {}});
}

auto DiGraph::removeNode(const Id &id) -> void
{
    std::erase_if(nodes, [&id](const auto &n)
                  { return n.data == id; });
}

auto DiGraph::removeEdge(const Id &parent, const Id &id) -> void
{
    auto *node = getNode(parent);
    if (node)
    {
        std::erase_if(node->edges, [&id](const auto &eRef)
                      { return eRef->data == id; });
    }
}

auto DiGraph::hasNode(const Id &id) const -> bool
{
    return getNode(id) != nullptr;
}

auto DiGraph::hasEdge(const Id &parent, const Id &child) const -> bool
{
    auto *node = getNode(parent);
    if (!node)
        return false;
    return std::any_of(node->edges.begin(), node->edges.end(), [&child](const auto &eRef)
                       { return eRef->data == child; });
}

auto DiGraph::printGraph() const -> void
{
    if (nodes.empty())
    {
        std::cout << "Graph is empty\n";
        return;
    }

    std::cout << "Graph:\n";
    for (const auto &node : nodes)
    {
        if (node.edges.empty())
        {
            std::cout << std::format("  Node 0x{:X} -> (no edges)\n", node.data);
        }
        else
        {
            std::cout << std::format("  Node 0x{:X} -> ", node.data);
            for (size_t i = 0; i < node.edges.size(); ++i)
            {
                std::cout << std::format("0x{:X}", node.edges[i]->data);
                if (i < node.edges.size() - 1)
                {
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

auto DiGraph::getNodes() -> std::vector<GraphNode> &
{
    return nodes;
}

auto DiGraph::clearEdges() -> void
{
    std::for_each(nodes.begin(), nodes.end(), [](auto &n)
                  { n.edges.clear(); });
}

auto DiGraph::topologicalSort() const -> std::optional<std::vector<Id>>
{
    if (nodes.empty())
    {
        return std::vector<Id>{};
    }

    std::unordered_map<Id, size_t> inDegree;
    for (const auto &node : nodes)
    {
        inDegree[node.data] = 0;
    }

    for (const auto &node : nodes)
    {
        for (const auto &edge : node.edges)
        {
            inDegree[edge->data]++;
        }
    }

    std::queue<Id> queue;
    for (const auto &[id, degree] : inDegree)
    {
        if (degree == 0)
        {
            queue.push(id);
        }
    }

    std::vector<Id> result;
    result.reserve(nodes.size());

    while (!queue.empty())
    {
        Id current = queue.front();
        queue.pop();
        result.push_back(current);

        auto *node = getNode(current);
        if (!node)
            continue;

        for (const auto &edge : node->edges)
        {
            inDegree[edge->data]--;
            if (inDegree[edge->data] == 0)
            {
                queue.push(edge->data);
            }
        }
    }

    if (result.size() != nodes.size())
    {
        return std::nullopt;
    }

    return result;
}

auto DiGraph::printTopologicalSort() const -> void
{
    auto result = topologicalSort();

    if (!result.has_value())
    {
        std::cout << "Graph has a cycle";
        return;
    }

    std::cout << "Topological order:\n";
    for (size_t i = 0; i < result->size(); ++i)
    {
        std::cout << std::format("  {}. 0x{:X}\n", i + 1, (*result)[i]);
    }
}

auto DiGraph::calcNodeDepths() const -> std::optional<std::unordered_map<Id, size_t>>
{
    if (nodes.empty())
    {
        return std::unordered_map<Id, size_t>{};
    }

    auto topoOrder = topologicalSort();
    if (!topoOrder.has_value())
    {
        return std::nullopt;
    }

    std::unordered_map<Id, size_t> depths;

    for (const auto &node : nodes)
    {
        depths[node.data] = 0;
    }

    for (const auto &nodeId : *topoOrder)
    {
        auto *node = getNode(nodeId);
        if (!node)
            continue;

        for (const auto &edge : node->edges)
        {
            depths[edge->data] = std::max(depths[edge->data], depths[nodeId] + 1);
        }
    }

    return depths;
}

auto DiGraph::getMaxDepth() const -> std::optional<size_t>
{
    auto depths = calcNodeDepths();
    if (!depths.has_value()) {
        return std::nullopt;
    }
    
    if (depths->empty()) {
        return 0;
    }
    
    size_t maxDepth = 0;
    for (const auto& [id, depth] : *depths) {
        maxDepth = std::max(maxDepth, depth);
    }
    
    return maxDepth;
}

auto DiGraph::getNodesAtDepth(size_t targetDepth) const -> std::vector<Id>
{
    auto depths = calcNodeDepths();
    if (!depths.has_value()) {
        return {};
    }
    
    std::vector<Id> result;
    for (const auto& [id, depth] : *depths) {
        if (depth == targetDepth) {
            result.push_back(id);
        }
    }
    
    return result;
}

auto DiGraph::printByDepth() const -> void
{
    auto depths = calcNodeDepths();
    
    if (!depths.has_value()) {
        std::cout << "Cannot calculate depths: graph contains a cycle\n";
        return;
    }
    
    auto maxDepth = getMaxDepth();
    if (!maxDepth.has_value()) {
        std::cout << "Graph is empty\n";
        return;
    }
    
    std::cout << "Nodes by depth:\n";
    for (size_t depth = 0; depth <= *maxDepth; ++depth) {
        auto nodesAtDepth = getNodesAtDepth(depth);
        
        std::cout << std::format("  Depth {}: ", depth);
        for (size_t i = 0; i < nodesAtDepth.size(); ++i) {
            std::cout << std::format("0x{:X}", nodesAtDepth[i]);
            if (i < nodesAtDepth.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
}
