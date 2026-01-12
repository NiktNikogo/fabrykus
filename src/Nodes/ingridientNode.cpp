#include "ingridientNode.hpp"

IngridientNode::IngridientNode() : SimpleMachineNode()
{
    ins.clear();
    outs.clear();
    outs.push_back({1, "Iron ore"});
    setTitle("Source");
    setStyle(ImFlow::NodeStyle::green());
    syncPins();
}

auto IngridientNode::draw() -> void
{
    ImGui::Text("UID: 0x%lX", this->getUID());
    
    ImGui::PushItemWidth(100.f);

    ImGui::Text("Time:");
    ImGui::SameLine();
    ImGui::InputDouble("##Time", &time);
   
    if (formatInputIngridients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
                               { this->dropOUT(uid); }))
    {
        return;
    }
}

auto IngridientNode::update() -> void
{
}

auto IngridientNode::syncPins() -> void
{
    std::vector<uintptr_t> inUids, outUids;
    for(auto& p: this->getIns()) inUids.push_back(p->getUid());
    for(auto& p: this->getOuts()) outUids.push_back(p->getUid());
    for(auto id : inUids) this->dropIN(id);
    for(auto id : outUids) this->dropOUT(id);
    inPins.clear();
    outPins.clear();

    for (size_t i = 0; i < outs.size(); i++)
    {
        auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() {
            return Ingredient{ this->outs[i].amount / time, this->outs[i].name };
        });
        
        p->renderer([this, i](ImFlow::Pin *p) {
            if (i < outs.size()) {
                ImGui::Text("%s", this->outs[i].name.c_str());
                p->drawSocket();
                p->drawDecoration();
            } 
        });
        outPins.push_back(p);
    }
}
