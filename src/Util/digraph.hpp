#pragma once

#include <ImNodeFlow.h>
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

};