#include "digraph.hpp"
#include <format>
#include <unordered_map>
#include <queue>

#include "boost/graph/connected_components.hpp"

auto DiGraph::addEdge(const Id &parent, const Id &child, float weight) -> void
{
    if (hasNode(parent) && hasNode(child))
    {
        boost::add_edge(idToVertex[parent], idToVertex[child], weight, graph);
    }
}

auto DiGraph::addNode(const Id &id) -> void
{
    if (idToVertex.find(id) == idToVertex.end())
    {
        auto v = boost::add_vertex(id, graph);
        idToVertex[id] = v;
        vertexToId[v] = id;
    }
}

auto DiGraph::getChildren(const Id &id) const -> std::vector<Id>
{
    std::vector<Id> children{};
    auto it = idToVertex.find(id);
    if (it == idToVertex.end())
        return children;

    Vertex u = it->second;
    auto [begin, end] = boost::out_edges(u, graph);

    for (auto el_it = begin; el_it != end; ++el_it)
    {
        Vertex v = boost::target(*el_it, graph);
        children.push_back(graph[v]);
    }
    return children;
}

auto DiGraph::getNodes() -> std::vector<Id>
{
    std::vector<Id> ids;
    for (auto const &[id, vertex] : idToVertex)
    {
        ids.push_back(id);
    }
    return ids;
}

auto DiGraph::getEdges() -> std::vector<Edge>
{
    std::vector<Edge> edges;
    for (auto e : boost::make_iterator_range(boost::edges(graph)))
    {
        auto u = boost::source(e, graph);
        auto v = boost::target(e, graph);
        edges.push_back({u, v});
    }
    return edges;
}

auto DiGraph::getComponents() -> std::vector<std::vector<Vertex>>
{
	std::vector<int> component(boost::num_vertices(graph));
    int numComponents = boost::connected_components(graph, &component[0]);

    std::vector<std::vector<Vertex>> groups(numComponents);
    for(size_t i = 0; i < component.size(); i++) {
        groups[component[i]].push_back(i);
    }
    
    return groups;
}

auto DiGraph::getIsolatedGraphs(ImFlow::ImNodeFlow &grid) -> std::vector<IsolatedGraph>
{
	auto components = getComponents();
    std::vector<IsolatedGraph> isoGraphs;
    for(const auto& comp : components ) {
        std::vector<Id> ids{};
        for(const auto& el : comp) {
            ids.push_back(vertexToId.at(el));
        }
        IsolatedGraph isoGraph(ids);
        isoGraph.findSources(grid);
        if(isoGraph.isProductionLine()) {
            isoGraphs.push_back(IsolatedGraph(isoGraph));
        }
    }

    return isoGraphs;
}

auto DiGraph::getParents(const Id &id) const -> std::vector<Id>
{
    std::vector<Id> parents;
    auto v = idToVertex.find(id);
    if (v == idToVertex.end())
        return parents;

    auto [begin, end] = boost::in_edges(v->second, graph);
    for (auto it = begin; it != end; it++)
    {
        auto parent = boost::source(*it, graph);
        auto parentIt = vertexToId.find(parent);
        if (parentIt != vertexToId.end())
        {
            parents.push_back(parentIt->second);
        }
    }
    return parents;
}

auto DiGraph::getParentsWithWeights(const Id &id) const -> std::vector<std::pair<Id, float>>
{
    std::vector<std::pair<Id, float>> parents;
    auto v = idToVertex.find(id);
    if (v == idToVertex.end())
        return parents;

    auto [begin, end] = boost::in_edges(v->second, graph);
    for (auto it = begin; it != end; it++)
    {
        auto parent = boost::source(*it, graph);
        auto parentIt = vertexToId.find(parent);
        if (parentIt != vertexToId.end())
        {
            parents.push_back({parentIt->second, graph[*it]});
        }
    }
    return parents;
}

auto DiGraph::removeNode(const Id &id) -> void
{
    auto it = idToVertex.find(id);
    if (it == idToVertex.end())
        return;
    boost::clear_vertex(it->second, graph);
    boost::remove_vertex(it->second, graph);
    idToVertex.erase(it);
    vertexToId.erase(it->second);
}

auto DiGraph::removeEdge(const Id &parent, const Id &child) -> void
{
    if (!hasNode(parent) || !hasNode(child))
        return;
    auto u = idToVertex.at(parent);
    auto v = idToVertex.at(child);

    auto edge = boost::edge(u, v, graph);
    if (!edge.second)
        return;
    boost::remove_edge(edge.first, graph);
}

auto DiGraph::hasNode(const Id &id) const -> bool
{
    return idToVertex.find(id) != idToVertex.end();
}

auto DiGraph::hasEdge(const Id &parent, const Id &child) const -> bool
{
    if (!hasNode(parent) || !hasNode(child))
        return false;
    auto u = idToVertex.at(parent);
    auto v = idToVertex.at(child);

    return boost::edge(u, v, graph).second;
}

auto DiGraph::printGraph() -> void
{
    if (idToVertex.empty())
    {
        std::cout << "Graph is empty\n";
        return;
    }

    std::cout << "Graph:\n";
    for (const auto &node : getNodes())
    {
        auto children = getChildren(node);
        if (children.empty())
        {
            std::cout << std::format("  Node 0x{:X} -> (no edges)\n", node);
        }
        else
        {
            std::cout << std::format("  Node 0x{:X} -> ", node);
            for (size_t i = 0; i < children.size(); ++i)
            {
                std::cout << std::format("0x{:X}", children[i]);
                if (i < children.size())
                {
                    std::cout << ", ";
                }
            }
            std::cout << "\n";
        }
    }
}

auto DiGraph::clearEdges() -> void
{
    auto [it, end] = boost::vertices(graph);
    for (; it != end; ++it)
    {
        boost::clear_vertex(*it, graph);
    }
}

auto DiGraph::topologicalSort() const -> std::optional<std::vector<Id>>
{
    if (idToVertex.size() < 1)
    {
        return std::vector<Id>{};
    }

    std::vector<Vertex> vertices;

    // czemu exception...
    try
    {
        boost::topological_sort(graph, std::back_inserter(vertices));
    }
    catch (const boost::not_a_dag &e)
    {
        return std::nullopt;
    }

    std::vector<Id> rev;
    rev.reserve(vertices.size());
    for (auto it = vertices.rbegin(); it != vertices.rend(); ++it)
    {
        if(vertexToId.contains(*it)) {
            rev.push_back(vertexToId.at(*it));
        }
    }

    return rev;
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

auto DiGraph::calcNodeDepths() -> std::optional<std::unordered_map<Id, size_t>>
{
    if (idToVertex.empty())
    {
        return std::unordered_map<Id, size_t>{};
    }

    auto topoOrder = topologicalSort();
    if (!topoOrder.has_value())
    {
        return std::nullopt;
    }

    std::unordered_map<Id, size_t> depths;

    for (const auto &node : getNodes())
    {
        depths[node] = 0;
    }

    for (const auto &nodeId : *topoOrder)
    {
        auto parents = getParents(nodeId);
        for (const auto &parent : parents) {
            depths[nodeId] = std::max(depths[nodeId], depths[parent] + 1);
        }
    }

    return depths;
}

auto DiGraph::getMaxDepth() -> std::optional<size_t>
{
    auto depths = calcNodeDepths();
    if (!depths.has_value())
    {
        return std::nullopt;
    }

    if (depths->empty())
    {
        return 0;
    }

    size_t maxDepth = 0;
    for (const auto &[id, depth] : *depths)
    {
        maxDepth = std::max(maxDepth, depth);
    }

    return maxDepth;
}

auto DiGraph::getNodesAtDepth(size_t targetDepth) -> std::vector<Id>
{
    auto depths = calcNodeDepths();
    if (!depths.has_value())
    {
        return {};
    }

    std::vector<Id> result;
    for (const auto &[id, depth] : *depths)
    {
        if (depth == targetDepth)
        {
            result.push_back(id);
        }
    }

    return result;
}

auto DiGraph::printByDepth() -> void
{
    auto depths = calcNodeDepths();

    if (!depths.has_value())
    {
        std::cout << "Cannot calculate depths: graph contains a cycle\n";
        return;
    }

    auto maxDepth = getMaxDepth();
    if (!maxDepth.has_value())
    {
        std::cout << "Graph is empty\n";
        return;
    }

    std::cout << "Nodes by depth:\n";
    for (size_t depth = 0; depth <= *maxDepth; ++depth)
    {
        auto nodesAtDepth = getNodesAtDepth(depth);

        std::cout << std::format("  Depth {}: ", depth);
        for (size_t i = 0; i < nodesAtDepth.size(); ++i)
        {
            std::cout << std::format("0x{:X}", nodesAtDepth[i]);
            if (i < nodesAtDepth.size() - 1)
            {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
}

auto DiGraph::calculatePositions(const std::map<Id, ImVec2> &nodeSizes) -> std::optional<std::map<Id, ImVec2>>
{
    auto depths = calcNodeDepths();
    if (!depths.has_value())
    {
        return std::nullopt;
    }

    size_t maxDepth = 0;
    for (const auto &[id, depth] : *depths)
    {
        maxDepth = std::max(maxDepth, depth);
    }

    std::map<size_t, std::vector<Id>> results;
    for (const auto &[id, depth] : *depths)
    {
        std::cout << std::format("{} {:#X} {}", id, id, depth) << '\n';
        auto parents = getParents(id);
        if (parents.size() > 0)
        {
            for (const auto &parent : parents)
            {
                std::cout << std::format("Parent: {}", parent) << '\n';
            }
        }
        results[depth].push_back(id);
    }

    std::map<Id, ImVec2> newPositions{};
    float currentX = 0.0f;
    float columnGap = 100.0f;
    float nodeGap = 40.0f;

    for (auto &[depth, elements] : results)
    {
        if (depth > 0)
        {
            std::sort(elements.begin(), elements.end(), [&](Id a, Id b)
                      {
                auto getAvgY = [&](Id id){
                    auto parents = getParentsWithWeights(id);
                    
                    if(parents.empty()) return 0.0f;

                    float sumY = 0.0f;
                    for(const auto& [parentId, weight] : parents){
                        std::cout << std::format("{}, {}", parentId, weight) << '\n';
                        sumY += newPositions[parentId].y + weight;
                    }
                    return sumY / parents.size();
                };        
            return getAvgY(a) < getAvgY(b); });
        }

        float maxColumnWidth = 0.0f;
        float totalColumnHeight = 0.0f;

        for (auto const &el : elements)
        {
            auto size = nodeSizes.at(el);
            maxColumnWidth = std::max(maxColumnWidth, size.x);
            totalColumnHeight += size.y;
        }
        totalColumnHeight += nodeGap * (elements.size() - 1);

        float currentY = -(totalColumnHeight / 2.0f);
        for (auto const &el : elements)
        {
            auto size = nodeSizes.at(el);
            newPositions[el] = ImVec2{currentX, currentY};
            currentY += size.y + nodeGap;
        }
        currentX += maxColumnWidth + columnGap;
    }

    return newPositions;
}
