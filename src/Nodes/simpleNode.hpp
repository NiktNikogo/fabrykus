#pragma once
#include <ImNodeFlow.h>
#include <format>
#include <vector>

template<unsigned int N>
class SimpleNode : public ImFlow::BaseNode {
private:
    int m_valB = 0;
    int m_valA = 0;
    int ins[N] = {0}; 
public:
    SimpleNode();
    void draw() override;

};
