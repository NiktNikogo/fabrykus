#pragma once

#include <nlohmann/json.hpp>

enum NodeType {
  MACHINE,
  PRODUCT,
  INGREDIENT,
  SPLITTER,
  MERGER
};

NLOHMANN_JSON_SERIALIZE_ENUM( NodeType, {
    {NodeType::MACHINE, "machine"},
    {NodeType::PRODUCT, "product"},
    {NodeType::INGREDIENT, "ingredient"},
    {NodeType::SPLITTER, "splitter"},
    {NodeType::MERGER, "merger"},
})