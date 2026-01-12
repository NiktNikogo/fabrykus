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

    auto LabelMatchFilter = [](ImFlow::Pin *out, ImFlow::Pin *in) -> bool
    {
        auto *outNode = dynamic_cast<SimpleMachineNode *>(out->getParent());
        auto *inNode = dynamic_cast<SimpleMachineNode *>(in->getParent());
        if (!outNode || !inNode)
            return false;

        size_t outIdx = out->getUid();
        size_t inIdx = in->getUid();

        return outNode->outs[outIdx].name == inNode->ins[inIdx].name;
    };

    setTitle("Machine");
    setStyle(ImFlow::NodeStyle::cyan());

    for (size_t i = 0; i < ins.size(); i++)
    {
        this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)
            ->renderer([this, i](ImFlow::Pin *p)
                       {
                ImGui::Text("%s", this->ins[i].name.c_str());
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration(); });
    }

    for (size_t i = 0; i < outs.size(); i++)
    {
        this->addOUT_uid<Ingredient>(i, " ")
            ->renderer([this, i](ImFlow::Pin *p)
                       {
            ImGui::Text("%s", this->outs[i].name.c_str());
            p->drawSocket();
            p->drawDecoration(); });
    }
}
SimpleMachineNode::SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
{
}
auto SimpleMachineNode::draw() -> void
{

    update();
    ImGui::Text("UID: 0x%lX", this->getUID());
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

    auto formatInputFields = [&](const char *category, std::vector<Ingredient> &list, const char *catPrefix)
    {
        ImGui::Text("%s", category);
        for (auto i = 0; i < list.size(); i++)
        {
            ImGui::Text("%d:", i);
            ImGui::SameLine();
            auto tmp = static_cast<int>(list[i].amount);
            if (ImGui::InputInt(std::format("##{}Amt{}", catPrefix, i).c_str(), &tmp))
            {
                list[i].amount = tmp >= 0 ? static_cast<size_t>(tmp) : list[i].amount;
            }
            ImGui::SameLine();
            char buffer[SimpleMachineNode::TEXT_INPUT_MAX_LENGTH]{};
            snprintf(buffer, sizeof(buffer), "%s", list[i].name.c_str());
            if (ImGui::InputText(std::format("##{}Name{}", catPrefix, i).c_str(), buffer, sizeof(buffer)))
            {
                list[i].name = std::string(buffer);
            }
        }
    };
    formatInputFields("Inputs:", ins, "in");
    formatInputFields("Outputs", outs, "out");
}

auto SimpleMachineNode::update() -> void
{
    for (size_t i = 0; i < ins.size(); i++)
    {
        auto p = inPin(i);
        if (!p)
            continue;
        if (p->isConnected())
        {
            auto linkPtr = p->getLink().lock();
            if (!linkPtr)
                continue;

            auto other = linkPtr->left();
            auto otherNode = dynamic_cast<SimpleMachineNode *>(other->getParent());

            if (otherNode)
            {
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
