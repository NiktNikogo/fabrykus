#pragma once
#include <memory>
#include <vector>
#include <ImNodeFlow.h>

struct GraphNode {
    ImFlow::NodeUID data;
    std::vector<std::shared_ptr<GraphNode>> edges;
};
