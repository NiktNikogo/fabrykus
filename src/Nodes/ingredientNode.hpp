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
    auto syncPins() -> void;
    auto update() -> void;
    auto drawInspector() -> bool override;
    auto deserialize(nlohmann::json data) -> void override;
    const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle> override;
};
