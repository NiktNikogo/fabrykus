#include "distributorNode.hpp"

DistributorNode::DistributorNode()
	: SimpleMachineNode()
{
}

DistributorNode::DistributorNode(size_t id)
	: SimpleMachineNode(id)
{
}

DistributorNode::DistributorNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
	: SimpleMachineNode(id, 1.0, 1.0, ins, outs)
{
}

auto DistributorNode::draw() -> void 
{
	return;
}

auto DistributorNode::syncPins() -> void 
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
		auto p = addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, labelMatchFilter)->renderer(
			[this, i](ImFlow::Pin *p) {
				if (i < getInList().size()) {
					auto recived = getInVal<Ingredient>(i);
					ImGui::Text("%s", getInList()[i].name.c_str());
					if(mode == Mode::MERGER) {
						auto intake = recived.asDouble()/totalIntake;
						getInList()[i].fromDouble(intake); 
						ImGui::TextDisabled("I: %.2f%%", getInList()[i].asDouble());
					} else if(mode == Mode::SPLITTER) {
						inAmount = recived.asDouble();
						ImGui::TextDisabled("I: %.2f", inAmount);
					}
					ImGui::SameLine();
					p->drawSocket();
					p->drawDecoration();
				} 
			});
		inPins.push_back(p);
	}
	for (size_t i = 0; i < getOutList().size(); i++)
	{

		auto p = addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() { 
			if(mode == Mode::MERGER) {
				totalIntake = 0.0f;
				for(size_t j = 0; j < getInList().size(); j++) {
					auto  ing = getInVal<Ingredient>(j);
					totalIntake += getInVal<Ingredient>(j).asDouble();
				}
				return Ingredient{totalIntake, getOutList()[i].name};
			} else {
				return Ingredient{getOutList()[i].asDouble() * inAmount, getOutList()[i].name}; 
			} });

		p->renderer([this, i](ImFlow::Pin *p)
					{
            if (i < getOutList().size()) {
                ImGui::Text("%s", getOutList()[i].name.c_str());
				if(mode == Mode::MERGER) ImGui::TextDisabled("O: %.2f", totalIntake);
				else ImGui::TextDisabled("O: %.2f%%", getOutList()[i].asDouble());
                p->drawSocket();
                p->drawDecoration();
            } });
		outPins.push_back(p);
	}
}
