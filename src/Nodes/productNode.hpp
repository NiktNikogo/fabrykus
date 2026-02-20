#pragma once

#include "Nodes/simpleMahcineNode.hpp"
#include "Util/nodeTypes.hpp"

class ProductNode : public SimpleMachineNode {

public:
    ProductNode();
    ProductNode(size_t id);
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void override;
    auto drawInspector() -> void override;
    const auto getNodeType() const -> NodeType override;
};
