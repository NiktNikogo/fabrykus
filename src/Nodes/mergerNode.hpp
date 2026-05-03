#pragma once

#include "Nodes/distributorNode.hpp"
#include "Util/nodeTypes.hpp"
#include "ImNodeFlow.h"

class MergerNode : public DistributorNode
{
private:
	static std::shared_ptr<ImFlow::NodeStyle> orange() { 
    return std::make_shared<ImFlow::NodeStyle>(
        IM_COL32(204, 85, 0, 255),  
        ImColor(255, 255, 255, 255),
        6.5f 
    ); 
}

public:
	MergerNode();
	MergerNode(size_t id);
	MergerNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
	auto draw() -> void override;
	auto update() -> void;
	auto drawInspector() -> bool override;
	auto deserialize(nlohmann::json data) -> void override;
	virtual const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle> override;
};