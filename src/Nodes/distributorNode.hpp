#pragma once

#include "Nodes/simpleMachineNode.hpp"
#include "Util/nodeTypes.hpp"
#include "ImNodeFlow.h"

class DistributorNode : public SimpleMachineNode
{
protected:
	enum Mode {
		SPLITTER,
		MERGER
	};

protected:
    double inAmount = 0.0f;
	double totalIntake = 0.0f;
    Mode mode;

public:
    DistributorNode();
    DistributorNode(size_t id);
    DistributorNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    virtual auto drawInspector() -> bool;
    virtual auto draw() -> void override;
    virtual auto syncPins() -> void override;
};