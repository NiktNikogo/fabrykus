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
	for (auto &p : getIns())
		inUids.push_back(p->getUid());
	for (auto &p : getOuts())
		outUids.push_back(p->getUid());
	for (auto id : inUids)
		dropIN(id);
	for (auto id : outUids)
		dropOUT(id);
	inPins.clear();
	outPins.clear();

	for (size_t i = 0; i < getInList().size(); i++)
    {
        auto p = addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, labelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < getInList().size()) {
                ImGui::Text("%s", getInList()[i].name.c_str());
                ImGui::TextDisabled("I: %.2f units/s ", getInVal<Ingredient>(i).asDouble());
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }

    for (size_t i = 0; i < getOutList().size(); i++)
    {
        auto p = addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() {
            return Ingredient{ getOutList()[i].asDouble() / time, getOutList()[i].name };
        });
        
        p->renderer([this, i](ImFlow::Pin *p) {
            if (i < getOutList().size()) {
                ImGui::Text("%s", getOutList()[i].name.c_str());
                ImGui::TextDisabled("O: %.2f units/s ", getOutList()[i].asDouble() /time);
                p->drawSocket();
                p->drawDecoration();
            } 
        });
        outPins.push_back(p);
    }
}
