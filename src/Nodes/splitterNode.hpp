#pragma once

#include "Nodes/distributorNode.hpp"
#include "Util/nodeTypes.hpp"
#include "ImNodeFlow.h"

class SplitterNode : public DistributorNode
{
private:

    static std::shared_ptr<ImFlow::NodeStyle> purple()
    {
        return std::make_shared<ImFlow::NodeStyle>(
            IM_COL32(128, 0, 128, 255), 
            ImColor(233, 241, 244, 255),
            6.5f
        );
    }

public:
    SplitterNode();
    SplitterNode(size_t id);
    SplitterNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    auto draw() -> void override;
    auto update() -> void;
    auto drawInspector() -> bool override;
    auto deserialize(nlohmann::json data) -> void override;
    virtual const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle> override;
};