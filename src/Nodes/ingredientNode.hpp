#pragma once
#include "Nodes/terminalNode.hpp"
#include "Util/nodeTypes.hpp"

class IngredientNode : public TerminalNode
{

public:
    IngredientNode();
    IngredientNode(size_t id);
    IngredientNode(size_t id, double time, std::vector<Ingredient> outs);
    auto draw() -> void override;
    auto drawInspector() -> bool override;
    const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle> override;
};
