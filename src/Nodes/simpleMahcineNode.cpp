#include "simpleMahcineNode.hpp"

#include <ImNodeFlow.h>

#include <format>
#include <string>


SimpleMachineNode::SimpleMachineNode() : fuel(0), time(0), inPins(), outPins{}
{

    ins = {{0, "Input 1"}, {0, "Input 2"}};
    outs = {{0, "Output 1"}, {0, "Output 2"}};
    setTitle("Machine");
    setStyle(ImFlow::NodeStyle::cyan());

    syncPins();
}
SimpleMachineNode::SimpleMachineNode(size_t time, size_t fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
{
}
auto SimpleMachineNode::draw() -> void
{

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

    if (formatInputIngridients("Inputs:", "in", ins, this->getIns(), [this](uintptr_t uid)
                               { this->dropIN(uid); }))
    {
        return;
    }
    if (formatInputIngridients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
                               { this->dropOUT(uid); }))
    {
        return;
    }

    update();
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

auto SimpleMachineNode::syncPins() -> void
{
 
    using Connection = std::pair<std::string, ImFlow::Pin*>;

    std::vector<Connection> savedInLinks;
    std::vector<Connection> savedOutLinks;
    for (auto &pin : this->getIns()) {
        if(pin->isConnected()) {
            if(auto link = pin->getLink().lock()) {
                auto uid = pin->getUid();
                if(uid < ins.size()) {
                    savedInLinks.push_back({ins[uid].name, link->left()});
                }
            }
        }
    }

    for(auto &pin : getOuts()) {
        if(pin->isConnected()) {
            if(auto link = pin->getLink().lock()) {
                auto uid = pin->getUid();
                if(uid < outs.size()) {
                    ImFlow::Pin* otherSide = link->right(); 
                    savedOutLinks.push_back({outs[uid].name, otherSide});
                }
            }
        }
    }
    std::vector<uintptr_t> inUids, outUids;
    for(auto& p: this->getIns()) inUids.push_back(p->getUid());
    for(auto& p: this->getOuts()) outUids.push_back(p->getUid());
    for(auto id : inUids) this->dropIN(id);
    for(auto id : outUids) this->dropOUT(id);
    inPins.clear();
    outPins.clear();

    auto LabelMatchFilter = [this](ImFlow::Pin *out, ImFlow::Pin *in) -> bool
    {
        auto *outNode = dynamic_cast<SimpleMachineNode *>(out->getParent());
        auto *inNode = dynamic_cast<SimpleMachineNode *>(in->getParent());
        if (!outNode || !inNode)
            return false;

        if (out->getUid() >= outNode->outs.size() || in->getUid() >= inNode->ins.size())
            return false;

        return outNode->outs[out->getUid()].name == inNode->ins[in->getUid()].name;
    };

    for (size_t i = 0; i < ins.size(); i++)
    {
        auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < ins.size()) {
                ImGui::Text("%s", this->ins[i].name.c_str());
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }

    for (size_t i = 0; i < outs.size(); i++)
    {
        auto p = this->addOUT_uid<Ingredient>(i, " ")->renderer([this, i](ImFlow::Pin *p)
                                                                {
            if (i < outs.size()) {
                ImGui::Text("%s", this->outs[i].name.c_str());
                p->drawSocket();
                p->drawDecoration();
            } });
        outPins.push_back(p);
    }

    
    for(size_t i = 0; i < ins.size(); i++) {
        for(auto& conn : savedInLinks) {
            if(conn.first == ins[i].name && conn.second) {
                inPins[i]->createLink(conn.second);
            }
        }
    }

    for(size_t i=0; i < outs.size(); i++) {
        for(auto& conn : savedOutLinks) {
            if(conn.first == outs[i].name && conn.second) {
                outPins[i]->createLink(conn.second);
            }
        }
    }
    
}

auto SimpleMachineNode::formatInputIngridients(const char *category, const char *prefix,
                                               std::vector<Ingredient> &list, 
                                               const std::vector<std::shared_ptr<ImFlow::Pin>>&pins, std::function<void(uintptr_t)> dropFunc) -> bool
{
    ImGui::Text("%s", category);
    ImGui::SameLine();
    if (ImGui::Button(std::format("+##Add{}", prefix).c_str()))
    {
        list.push_back({0, "New"});
        this->syncPins();
        return true;
    }

    for (auto i = 0; i < list.size(); i++)
    {
        ImGui::PushID(i);
        ImGui::Text("%d:", i);
        ImGui::SameLine();
        auto tmp = static_cast<int>(list[i].amount);
        if (ImGui::InputInt(std::format("##{}Amt{}", prefix, i).c_str(), &tmp))
        {
            list[i].amount = tmp >= 0 ? static_cast<size_t>(tmp) : list[i].amount;
        }
        ImGui::SameLine();
        char buffer[SimpleMachineNode::TEXT_INPUT_MAX_LENGTH]{};
        snprintf(buffer, sizeof(buffer), "%s", list[i].name.c_str());
        if (ImGui::InputText(std::format("##{}Name{}", prefix, i).c_str(), buffer, sizeof(buffer)))
        {
            list[i].name = std::string(buffer);
        }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button(std::format("X##{}_Del_{}", prefix, i).c_str()))
        {
            dropFunc(pins[i]->getUid());
            list.erase(list.begin() + i);
            this->syncPins();
            ImGui::PopStyleColor();
            ImGui::PopID();
            return true;
        }
        ImGui::PopStyleColor();
        ImGui::PopID();
    }
    return false;
}