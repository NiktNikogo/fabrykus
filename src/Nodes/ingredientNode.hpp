#pragma once
#include "Nodes/simpleMahcineNode.hpp"
#include "Util/nodeTypes.hpp"

class IngredientNode : public SimpleMachineNode
{

public:
    IngredientNode();
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void override;
    auto drawInspector() -> void override;
    const auto getNodeType() -> NodeType const override;
};
