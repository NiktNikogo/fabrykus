#pragma once

#include "Nodes/terminalNode.hpp"
#include "Util/nodeTypes.hpp"

class ProductNode : public TerminalNode {

public:
    ProductNode();
    ProductNode(size_t id);
    ProductNode(size_t id, std::vector<Ingredient> ins);
    auto draw() -> void override;
    auto drawInspector() -> bool override;
    const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle> override;
};
