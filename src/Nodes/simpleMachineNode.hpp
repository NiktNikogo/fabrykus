#pragma once

#include <ImNodeFlow.h>
#include <memory>
#include <nlohmann/json.hpp>

#include "Util/ingredient.hpp"
#include "Util/nodeTypes.hpp"

class SimpleMachineNode : public ImFlow::BaseNode
{

protected:
    size_t id = -1;
    double time;
    double fuel;
    std::vector<Ingredient> ins;
    std::vector<Ingredient> outs;
    std::vector<ImFlow::Pin *> inPins;
    std::vector<ImFlow::Pin *> outPins;

public:
    static const size_t TEXT_INPUT_MAX_LENGTH = 128;
    NodeType type = NodeType::MACHINE;
protected:
    auto formatInputIngredients(const char *category, const char *prefix, std::vector<Ingredient> &list,
                                const std::vector<std::shared_ptr<ImFlow::Pin>> &pins, std::function<void(uintptr_t)> dropFunc,
                                bool canAdd = true) -> bool;
    auto calcEfficiency() -> double;
    auto calcOptimalCount() -> double;

public:
    SimpleMachineNode();
    SimpleMachineNode(size_t id);
    SimpleMachineNode(size_t id, double time, double fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    virtual ~SimpleMachineNode() = default;
    auto draw() -> void override;
    virtual auto drawInspector() -> bool;
    virtual auto update() -> void;
    virtual auto syncPins() -> void;
    inline const std::vector<Ingredient> &getInList() const { return ins; }
    inline const std::vector<Ingredient> &getOutList() const { return outs; }
    inline ImFlow::Pin* getInListElement(size_t idx) const {return inPins[idx];}
    inline ImFlow::Pin* getOutListElement(size_t idx) const {return outPins[idx];}
    inline const size_t getId() const { return id; }
    const auto print() const -> void;
    virtual const auto getNodeType() const -> NodeType {return type;};
    virtual auto serialize() -> nlohmann::json;
    virtual auto deserialize(nlohmann::json data) -> void;
    const auto getInPinIndex(ImFlow::Pin *pin) const -> size_t;
    const auto getOutPinIndex(ImFlow::Pin *pin) const -> size_t;    
};