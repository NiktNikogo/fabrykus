#pragma once

#include "Nodes/distributorNode.hpp"
#include "Util/nodeTypes.hpp"
#include "ImNodeFlow.h"

class SplitterNode : public DistributorNode
{
public:
    SplitterNode();
    SplitterNode(size_t id);
    SplitterNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    auto draw() -> void override;
    auto update() -> void;
    auto drawInspector() -> bool override;
    virtual const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle> override;
};