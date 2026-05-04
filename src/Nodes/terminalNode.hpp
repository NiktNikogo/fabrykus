#pragma once
#include "Nodes/simpleMachineNode.hpp"
#include "Util/nodeTypes.hpp"

class TerminalNode : public SimpleMachineNode
{
protected:
	enum Mode {
		INGREDIENT,
		PRODUCT
	};
protected:
	Mode mode;

public:
    TerminalNode();
    TerminalNode(size_t id);
    TerminalNode(size_t id, double time, std::vector<Ingredient> outs);
    auto draw() -> void override;
    auto syncPins() -> void;
 };
