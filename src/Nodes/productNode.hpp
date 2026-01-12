#pragma once

#include "Nodes/simpleMahcineNode.hpp"

class ProductNode : public SimpleMachineNode {

public:
    ProductNode();
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void override;
};