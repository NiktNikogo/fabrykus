#include "simpleMahcineNode.hpp"

#include "ImNodeFlow.h"
#include <format>
SimpleMachineNode::SimpleMachineNode() : fuel(0), time(0)
{
    
    setTitle("Machine");
    setStyle(ImFlow::NodeStyle::cyan());
    ImFlow::BaseNode::addIN<Ingredient>("Input 1", Ingredient{0, ""}, ImFlow::ConnectionFilter::SameType())->renderer([this](ImFlow::Pin* p) {
        auto pp = dynamic_cast<ImFlow::InPin<float>*>(p);
        ImGui::Text(this->inName1);
        p->drawSocket();
        p->drawDecoration();
    });

    ImFlow::BaseNode::addIN<Ingredient>("Input 1", Ingredient{0, ""}, ImFlow::ConnectionFilter::SameType())->renderer([this](ImFlow::Pin* p) {
        auto pp = dynamic_cast<ImFlow::InPin<float>*>(p);
        ImGui::Text(this->inName2);
        p->drawSocket();
        p->drawDecoration();
    });

    ImFlow::BaseNode::addOUT<Ingredient>("Output 1", nullptr)->renderer([this](ImFlow::Pin* p) {
        auto pp = dynamic_cast<ImFlow::InPin<float>*>(p);
        ImGui::Text(this->outName1);
        p->drawSocket();
        p->drawDecoration();
    });

    ImFlow::BaseNode::addOUT<Ingredient>("Output 1", nullptr)->renderer([this](ImFlow::Pin* p) {
        auto pp = dynamic_cast<ImFlow::InPin<float>*>(p);
        ImGui::Text(this->outName2);
        p->drawSocket();
        p->drawDecoration();
    });


    
    ins.resize(2);
    outs.resize(2);
}
SimpleMachineNode::SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
{
}
auto SimpleMachineNode::draw() -> void
{

    ImGui::Text(std::format("UID: 0x{:X}", this->getUID()).c_str());
    ImGui::Text("Internal Pin Count: %d", (int)this->getIns().size() + (int)this->getOuts().size());
    ImGui::PushItemWidth(100.f);

    ImGui::Text("Time:");
    ImGui::SameLine();
    if (ImGui::InputInt("##Time", &timeInput))
    {
        time = static_cast<size_t>(std::max(0, timeInput));
    }

    ImGui::Text("Fuel:");
    ImGui::SameLine();
    if (ImGui::InputInt("##Fuel", &fuelInput))
    {
        fuel = static_cast<size_t>(std::max(0, fuelInput));
    }


    ImGui::Text("Inputs:");

    ImGui::Text("1:");
    ImGui::SameLine();
    if (ImGui::InputInt("##InAmt1", &inAmount1))
    {
        ins[0].amount = static_cast<size_t>(std::max(0, inAmount1));
    }
    ImGui::SameLine();
    if (ImGui::InputText("##InName1", inName1, sizeof(inName1)))
    {
        ins[0].name = inName1;
    }

    ImGui::Text("2:");
    ImGui::SameLine();
    if (ImGui::InputInt("##InAmt2", &inAmount2))
    {
        ins[1].amount = static_cast<size_t>(std::max(0, inAmount2));
    }
    ImGui::SameLine();
    if (ImGui::InputText("##InName2", inName2, sizeof(inName2)))
    {
        ins[1].name = inName2;
    }


    ImGui::Text("Outputs:");

    ImGui::Text("1:");
    ImGui::SameLine();
    if (ImGui::InputInt("##OutAmt1", &outAmount1))
    {
        outs[0].amount = static_cast<size_t>(std::max(0, outAmount1));
    }
    ImGui::SameLine();
    if (ImGui::InputText("##OutName1", outName1, sizeof(outName1)))
    {
        outs[0].name = outName1;
    }

    ImGui::Text("2:");
    ImGui::SameLine();
    if (ImGui::InputInt("##OutAmt2", &outAmount2))
    {
        outs[1].amount = static_cast<size_t>(std::max(0, outAmount2));
    }
    ImGui::SameLine();
    if (ImGui::InputText("##OutName2", outName2, sizeof(outName2)))
    {
        outs[1].name = outName2;
    }
}