#pragma once

#include <ImNodeFlow.h>
#include "Util/ingredient.hpp"

class SimpleMachineNode : public ImFlow::BaseNode {
private:
    size_t time;
    size_t fuel;
    std::vector<Ingredient> ins;
    std::vector<Ingredient> outs;

    char inName1[64] = "";
    char inName2[64] = "";
    char outName1[64] = "";
    char outName2[64] = "";
    int inAmount1 = 0;
    int inAmount2 = 0;
    int outAmount1 = 0;
    int outAmount2 = 0;
    int timeInput = 0;
    int fuelInput = 0;

public:
    SimpleMachineNode();
    SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    auto draw() -> void override ;
};  