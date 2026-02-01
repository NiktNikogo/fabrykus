#pragma once

#include <ImNodeFlow.h>
#include <memory>
#include "Util/ingredient.hpp"

class SimpleMachineNode : public ImFlow::BaseNode
{
protected:
    double time;
    double fuel;
    std::vector<Ingredient> ins;
    std::vector<Ingredient> outs;
    std::vector<ImFlow::Pin *> inPins;
    std::vector<ImFlow::Pin *> outPins;

public:
    static const size_t TEXT_INPUT_MAX_LENGTH = 128;

protected:
    auto formatInputIngridients(const char *category, const char *prefix, std::vector<Ingredient> &list,
                                const std::vector<std::shared_ptr<ImFlow::Pin>> &pins, std::function<void(uintptr_t)> dropFunc) -> bool;
    auto calcEfficiency() -> double;
    auto calcOptimalCount() -> double;
public:
    SimpleMachineNode();
    SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    virtual ~SimpleMachineNode() = default;
    auto draw() -> void override;
    virtual auto drawInspector() -> void;
    virtual auto update() -> void;
    virtual auto syncPins() -> void;
    inline const std::vector<Ingredient>& getInList() const { return ins; }
    inline const std::vector<Ingredient>& getOutList() const { return outs; }
    const auto print() -> void const;
};