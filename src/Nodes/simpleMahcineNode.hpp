#pragma once

#include <ImNodeFlow.h>
#include <memory>
#include "Util/ingredient.hpp"

class SimpleMachineNode : public ImFlow::BaseNode
{
private:
    size_t time;
    size_t fuel;
    std::vector<Ingredient> ins;
    std::vector<Ingredient> outs;
    std::vector<ImFlow::Pin *> inPins;
    std::vector<ImFlow::Pin *> outPins;
    int timeInput = 0;
    int fuelInput = 0;

public:
    static const size_t TEXT_INPUT_MAX_LENGTH = 128;

private:
    auto formatInputIngridients(const char *category, const char *prefix, std::vector<Ingredient> &list,
                                const std::vector<std::shared_ptr<ImFlow::Pin>> &pins, std::function<void(uintptr_t)> dropFunc) -> bool;

public:
    SimpleMachineNode();
    SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    auto draw() -> void override;
    auto update() -> void;
    auto syncPins() -> void;
};