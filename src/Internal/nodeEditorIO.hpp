#pragma once

#include <nlohmann/json.hpp>
#include <ImNodeFlow.h>

#include "Util/digraph.hpp"

class NodeEditorIO {
private:
    static auto serializeLinks(ImFlow::ImNodeFlow& grid) -> nlohmann::json;
    static auto collectData(ImFlow::ImNodeFlow& grid) -> nlohmann::json;
    static auto parseNodes(ImFlow::ImNodeFlow& grid, nlohmann::json nodes, DiGraph& digraph, size_t maxCurrId) -> size_t;
    static auto parseLinks(ImFlow::ImNodeFlow& grid, nlohmann::json links, DiGraph& digraph, size_t maxCurrId) -> void;
public:
    static auto save(const std::string &path, ImFlow::ImNodeFlow& grid) -> void; 
    static auto load(const std::string &path, ImFlow::ImNodeFlow& grid, DiGraph& digraph, size_t maxCurrId) -> size_t;
};