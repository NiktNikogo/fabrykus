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

	auto LabelMatchFilter = [this](ImFlow::Pin *out, ImFlow::Pin *in) -> bool
	{
		auto *outNode = dynamic_cast<SimpleMachineNode *>(out->getParent());
		auto *inNode = dynamic_cast<SimpleMachineNode *>(in->getParent());
		if (!outNode || !inNode)
			return false;

		if (out->getUid() >= outNode->getOutList().size() || in->getUid() >= inNode->getInList().size())
			return false;

		return outNode->getOutList()[out->getUid()].name == inNode->getInList()[in->getUid()].name;
	};
	for (size_t i = 0; i < getInList().size(); i++)
	{
		auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer(
			[this, i](ImFlow::Pin *p) {
				if (i < getInList().size()) {
					auto recived = getInVal<Ingredient>(i);
					ImGui::Text("%s", this->getInList()[i].name.c_str());
					if(mode == Mode::MERGER) {
						auto intake = recived.amount/totalIntake;
						getInList()[i].amount = intake; 
						ImGui::TextDisabled("I: %.2f%%", getInList()[i].amount);
					} else if(mode == Mode::SPLITTER) {
						this->inAmount = recived.amount;
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

		auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() { 
			if(mode == Mode::MERGER) {
				this->totalIntake = 0.0f;
				for(size_t j = 0; j < getInList().size(); j++) {
					auto  ing = getInVal<Ingredient>(j);
					totalIntake += getInVal<Ingredient>(j).amount;
				}
				return Ingredient{totalIntake, this->getOutList()[i].name};
			} else {
				return Ingredient{this->getOutList()[i].amount * this->inAmount, this->getOutList()[i].name}; 
			} });

		p->renderer([this, i](ImFlow::Pin *p)
					{
            if (i < getOutList().size()) {
                ImGui::Text("%s", this->getOutList()[i].name.c_str());
				if(mode == Mode::MERGER) ImGui::TextDisabled("O: %.2f%%", totalIntake);
				else ImGui::TextDisabled("O: %.2f%%", this->getOutList()[i].amount);
                p->drawSocket();
                p->drawDecoration();
            } });
		outPins.push_back(p);
	}
}
