#pragma once
#include "Nodes/simpleMachineNode.hpp"
#include "Util/nodeTypes.hpp"

class IngredientNode : public SimpleMachineNode
{

public:
    IngredientNode();
    IngredientNode(size_t id);
    IngredientNode(size_t id, double time, std::vector<Ingredient> outs);
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void override;
    auto drawInspector() -> void override;
    auto deserialize(nlohmann::json data) -> void override;
};
