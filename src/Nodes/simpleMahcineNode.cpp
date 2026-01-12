#include "simpleMahcineNode.hpp"

#include "ImNodeFlow.h"
#include <format>
#include <string>

SimpleMachineNode::SimpleMachineNode() : fuel(0), time(0)
{

    ins.resize(2);
    outs.resize(2);
    ins[0].name = std::string(inName1);
    ins[1].name = std::string(inName2);

    outs[0].name = std::string(outName1);
    outs[1].name = std::string(outName2);

    auto LabelMatchFilter = [](ImFlow::Pin* out, ImFlow::Pin* in) -> bool {
        auto* outNode = dynamic_cast<SimpleMachineNode*>(out->getParent());
        auto* inNode = dynamic_cast<SimpleMachineNode*>(in->getParent());
        if (!outNode || !inNode) return false;

        size_t outIdx = out->getUid(); 
        size_t inIdx = in->getUid();

        return outNode->outs[outIdx].name == inNode->ins[inIdx].name;
    };


    setTitle("Machine");
    setStyle(ImFlow::NodeStyle::cyan());

    for(size_t i = 0; i < ins.size(); i++) {
        this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)
            ->renderer([this, i](ImFlow::Pin* p) {
                ImGui::Text("%s", this->ins[i].name.c_str());
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
        });
    }

    for(size_t i = 0; i < outs.size(); i++) {
        this->addOUT_uid<Ingredient>(i, " ")
        ->renderer([this, i](ImFlow::Pin* p) {
            ImGui::Text("%s", this->outs[i].name.c_str());
            p->drawSocket();
            p->drawDecoration();
        });
    }
}
SimpleMachineNode::SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
{
    
}
auto SimpleMachineNode::draw() -> void
{
    update();
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

auto SimpleMachineNode::update() -> void {
    for(size_t i = 0; i < ins.size(); i++) {
        auto p = inPin(i);
        if(!p) continue; 
        if(p->isConnected()) {
            auto linkPtr = p->getLink().lock();
            if(!linkPtr) continue;

            auto other = linkPtr->left();
            auto otherNode = dynamic_cast<SimpleMachineNode*>(other->getParent());

            if(otherNode) {
                size_t otherIdx = other->getUid();
                
                std::string myInputName = this->ins[i].name;
                std::string theirOutputName = otherNode->outs[otherIdx].name;

                if (myInputName != theirOutputName)
                {
                    p->deleteLink();
                }
            }
        }
    }
}
