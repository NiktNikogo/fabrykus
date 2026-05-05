#include "terminalNode.hpp"

TerminalNode::TerminalNode()
	: SimpleMachineNode()
{
}

TerminalNode::TerminalNode(size_t id)
	: SimpleMachineNode(id)
{
}

TerminalNode::TerminalNode(size_t id, double time, std::vector<Ingredient> outs)
	: SimpleMachineNode(id, time, fuel, ins, outs)
{
}

auto TerminalNode::draw() -> void
{
	return;
}

auto TerminalNode::syncPins() -> void
{
	std::vector<uintptr_t> inUids, outUids;
	for (auto &p : this->getIns())
		inUids.push_back(p->getUid());
	for (auto &p : this->getOuts())
		outUids.push_back(p->getUid());
	for (auto id : inUids)
		this->dropIN(id);
	for (auto id : outUids)
		this->dropOUT(id);
	inPins.clear();
	outPins.clear();

	for (size_t i = 0; i < getInList().size(); i++)
    {
        auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, labelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < getInList().size()) {
                ImGui::Text("%s", this->getInList()[i].name.c_str());
                ImGui::TextDisabled("I: %.2f units/s ", getInVal<Ingredient>(i).amount);
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }

    for (size_t i = 0; i < getOutList().size(); i++)
    {
        auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() {
            return Ingredient{ this->getOutList()[i].amount / time, this->getOutList()[i].name };
        });
        
        p->renderer([this, i](ImFlow::Pin *p) {
            if (i < getOutList().size()) {
                ImGui::Text("%s", this->getOutList()[i].name.c_str());
                ImGui::TextDisabled("O: %.2f units/s ", getOutList()[i].amount/time);
                p->drawSocket();
                p->drawDecoration();
            } 
        });
        outPins.push_back(p);
    }
}
