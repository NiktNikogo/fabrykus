#pragma once

#include "Nodes/simpleMahcineNode.hpp"
#include "Util/nodeTypes.hpp"

class ProductNode : public SimpleMachineNode {

public:
    ProductNode();
    ProductNode(size_t id);
    ProductNode(size_t id, std::vector<Ingredient> ins);
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void override;
    auto drawInspector() -> void override;
    auto deserialize(nlohmann::json data) -> void override;
    const auto getNodeType() const -> NodeType override;
};
