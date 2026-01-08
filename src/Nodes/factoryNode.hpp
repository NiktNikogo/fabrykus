#pragma once
#include <ImNodeFlow.h>
#include <utility>
#include <string>
class FactoryNode : public ImFlow::BaseNode{
private:
    std::map<std::string, int> ins;
    std::map<std::string, int> outs;
    std::map<std::string, int> scaleVer;
    std::map<std::string, int> scaleHor;
public:
    FactoryNode();
    

    inline void addToIns(std::string name, int cost) {
        ins.insert({name,   cost});
    }

    inline void addToOuts(std::string name, int quantity) {
        outs.insert({name, quantity});
    }

    inline void addToVer(std::string name, int verValue) {
        scaleVer.insert({name, verValue});
    }

    inline void addToHer(std::string name, int herValue) {
        scaleHor.insert({name, herValue});
    }
};