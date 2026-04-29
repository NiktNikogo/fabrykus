#pragma once

#include <ImNodeFlow.h>
#include <nlohmann/json.hpp>

#include "Nodes/ingredientNode.hpp"
#include "Nodes/productNode.hpp"
#include "Nodes/simpleMachineNode.hpp"
#include "Nodes/splitterNode.hpp"
#include "Util/nodeTypes.hpp"
#include "digraph.hpp"

namespace NodeFactory
{
    inline auto addNode(NodeType type, size_t id, ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void
    {
        switch (type)
        {
        case NodeType::INGREDIENT:
            digraph.addNode(grid.placeNode<IngredientNode>(id)->getUID());
            break;
        case NodeType::MACHINE:
            digraph.addNode(grid.placeNode<SimpleMachineNode>(id)->getUID());
            break;
        case NodeType::PRODUCT:
            digraph.addNode(grid.placeNode<ProductNode>(id)->getUID());
            break;
        case NodeType::SPLITTER:
            digraph.addNode(grid.placeNode<SplitterNode>(id)->getUID());
        default:
            return;
        }
    }

    inline auto createNode(NodeType type, nlohmann::json data, ImFlow::ImNodeFlow &grid, DiGraph &digraph) -> void
    {
        switch (type)
        {
        case NodeType::INGREDIENT:
        {
            auto node = grid.placeNode<IngredientNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        case NodeType::MACHINE:
        {
            auto node = grid.placeNode<SimpleMachineNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        case NodeType::PRODUCT:
        {
            auto node = grid.placeNode<ProductNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        case NodeType::SPLITTER:
        {
            auto node = grid.placeNode<SplitterNode>();
            node->deserialize(data);
            digraph.addNode(node->getUID());
        }
        break;
        default:
            return;
        }
    }

    struct typeNameItem
    {
        NodeType type;
        std::string name;
    };

    inline const std::vector<typeNameItem> typeNameList = {
        {NodeType::MACHINE, "Machine",},
        {NodeType::PRODUCT, "Storage"},
        {NodeType::INGREDIENT, "Source"},
        {NodeType::SPLITTER, "Splitter"}
    };

    inline const std::map<NodeType, std::string> typeNameMap = {
        {NodeType::MACHINE, "Machine",},
        {NodeType::PRODUCT, "Storage"},
        {NodeType::INGREDIENT, "Source"},
        {NodeType::SPLITTER, "Splitter"}

    };

    inline auto getNameFromType(NodeType type) -> std::string {
        if(typeNameMap.contains(type)) {
            return typeNameMap.find(type)->second;
        }
        return "";
    }
}