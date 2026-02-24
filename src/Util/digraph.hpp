#pragma once

#include <ImNodeFlow.h>
#include <optional>
#include <unordered_map>
#include <imgui.h>

#include "graphNode.hpp"


class DiGraph
{
private:
    std::vector<GraphNode> nodes;

public:
    using Id = ImFlow::NodeUID;
    DiGraph() : nodes() {};

    auto getNode(const Id &id) -> GraphNode *;
    auto getNode(const Id &id) const -> const GraphNode *;
    auto addEdge(const Id &parent, const Id &child) -> void;
    auto addNode(const Id &id) -> void;
    auto removeNode(const Id &id) -> void;
    auto removeEdge(const Id &parent, const Id &id) -> void;
    auto hasNode(const Id &id) const -> bool;
    auto hasEdge(const Id &parent, const Id &child) const -> bool;
    auto printGraph() const -> void;
    auto getNodes() const -> const std::vector<GraphNode>& ;
    auto getNodes() -> std::vector<GraphNode>&; 
    auto clearEdges() -> void;
    auto topologicalSort() const -> std::optional<std::vector<Id>>;
    auto printTopologicalSort() const -> void;
    auto calcNodeDepths() const -> std::optional<std::unordered_map<Id, size_t>>;
    auto getMaxDepth() const -> std::optional<size_t>;
    auto getNodesAtDepth(size_t targetDepth) const -> std::vector<Id>;
    auto printByDepth() const -> void;
    auto calculatePositions(const std::map<Id, ImVec2>& nodeSizes) const -> std::optional<std::map<Id, ImVec2>>;
};