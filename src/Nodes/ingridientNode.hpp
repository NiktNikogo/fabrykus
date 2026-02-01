#pragma once
#include "Nodes/simpleMahcineNode.hpp"

class IngridientNode : public SimpleMachineNode
{

public:
    IngridientNode();
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void override;
    auto drawInspector() -> void override;
};
