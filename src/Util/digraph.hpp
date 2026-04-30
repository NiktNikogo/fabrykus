#pragma once

#include <ImNodeFlow.h>
#include <optional>
#include <unordered_map>
#include <imgui.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

#include "graphNode.hpp"
#include "Util/isolatedGraph.hpp"

class DiGraph
{
public:
    using Id = ImFlow::NodeUID;
    using Edge = std::pair<Id, Id>;

private:
    typedef boost::adjacency_list<boost::listS, boost::vecS, boost::bidirectionalS, Id, float> BoostGraph;
    typedef boost::graph_traits<BoostGraph>::vertex_descriptor Vertex;

    BoostGraph graph;
    std::unordered_map<Id, BoostGraph::vertex_descriptor> idToVertex;
    std::unordered_map<BoostGraph::vertex_descriptor, Id> vertexToId;

public:
    DiGraph() = default;

    auto addEdge(const Id &parent, const Id &child, float weight) -> void;
    auto addNode(const Id &id) -> void;

    auto getChildren(const Id &id) const -> std::vector<Id>;
    auto getNodes() -> std::vector<Id>;
    auto getEdges() -> std::vector<Edge>;
    auto getComponents() -> std::vector<std::vector<Vertex>>;
    auto getIsolatedGraphs(ImFlow::ImNodeFlow &grid) -> std::vector<IsolatedGraph>;

    auto getParents(const Id &id) const -> std::vector<Id>;
    auto getParentsWithWeights(const Id &id) const -> std::vector<std::pair<Id, float>>;

    auto removeNode(const Id &id) -> void;
    auto removeEdge(const Id &parent, const Id &child) -> void;

    auto hasNode(const Id &id) const -> bool;
    auto hasEdge(const Id &parent, const Id &child) const -> bool;

    auto printGraph() -> void;

    auto clearEdges() -> void;
    auto topologicalSort() const -> std::optional<std::vector<Id>>;
    auto printTopologicalSort() const -> void;
    auto calcNodeDepths() -> std::optional<std::unordered_map<Id, size_t>>;
    auto getMaxDepth() -> std::optional<size_t>;
    auto getNodesAtDepth(size_t targetDepth) -> std::vector<Id>;
    auto printByDepth() -> void;
    auto calculatePositions(const std::map<Id, ImVec2> &nodeSizes) -> std::optional<std::map<Id, ImVec2>>;
};