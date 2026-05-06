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

auto DistributorNode::drawInspector() -> bool
{
	bool isMerging = (mode == Mode::MERGER && !isReverseFlow) || (mode == Mode::SPLITTER && isReverseFlow);
	bool isSplitting = (mode == Mode::SPLITTER && !isReverseFlow) || (mode == Mode::MERGER && isReverseFlow);
	if (isMerging)
	{
		
		if (formatInputIngredients("Output:", "out", getOutList(), this->getOuts(), [this](uintptr_t uid)
							   { this->dropOUT(uid); }, {false, false, false, true, &getInList()}))
		{
			return true;
		}
		if (formatInputIngredients("Inputs:", "in", getInList(), this->getIns(), [this](uintptr_t uid)
								{ this->dropIN(uid); }, {false, true, true, true, &getOutList(), true, Rational(1,1) }))
		{
			return true;
		}
	}
	else
	{
		if (formatInputIngredients("Inputs:", "in", getInList(), this->getIns(), [this](uintptr_t uid)
								   { this->dropIN(uid); }, {false, false, false, true, &getOutList()}))
		{
			return true;
		}
		if (formatInputIngredients("Output:", "out", getOutList(), this->getOuts(), [this](uintptr_t uid)
								   { this->dropOUT(uid); }, {true, true, true, true, &getInList(), true, Rational(1, 1)}))
		{
			return true;
		}
	}
	return false;
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
			auto p = addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, labelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
																								  {
				bool isMerging = (mode == Mode::MERGER && !isReverseFlow) || (mode == Mode::SPLITTER && isReverseFlow);
				bool isSplitting = (mode == Mode::SPLITTER && !isReverseFlow) || (mode == Mode::MERGER && isReverseFlow);

				if (i < getInList().size()) {
					auto recived = getInVal<Ingredient>(i);
					ImGui::Text("%s", getInList()[i].name.c_str());
					if(isMerging) {
						auto intake = 0.0;
						intake = totalIntake > 0 ? recived.asDouble()/totalIntake : intake;
						getInList()[i].fromDouble(intake); 
						ImGui::TextDisabled("I: %.2f%%", getInList()[i].asDouble() * 100);
					} else {
						inAmount = recived.asDouble();
						ImGui::TextDisabled("I: %.2f", inAmount);
					}
					ImGui::SameLine();
					p->drawSocket();
					p->drawDecoration();
				} });
			inPins.push_back(p);
		}
		for (size_t i = 0; i < getOutList().size(); i++)
		{

			auto p = addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]()
															   { 
			bool isMerging = (mode == Mode::MERGER && !isReverseFlow) || (mode == Mode::SPLITTER && isReverseFlow);
			bool isSplitting = (mode == Mode::SPLITTER && !isReverseFlow) || (mode == Mode::MERGER && isReverseFlow);

			if(isMerging) {
				return Ingredient{totalIntake, getOutList()[i].name};
			} else {
				return Ingredient{getOutList()[i].asDouble() * inAmount, getOutList()[i].name}; 
			} });

			p->renderer([this, i](ImFlow::Pin *p)
						{
			bool isMerging = (mode == Mode::MERGER && !isReverseFlow) || (mode == Mode::SPLITTER && isReverseFlow);
			bool isSplitting = (mode == Mode::SPLITTER && !isReverseFlow) || (mode == Mode::MERGER && isReverseFlow);

            if (i < getOutList().size()) {
                ImGui::Text("%s", getOutList()[i].name.c_str());
				if(isMerging) ImGui::TextDisabled("O: %.2f", totalIntake);
				else ImGui::TextDisabled("O: %.2f%%", getOutList()[i].asDouble() * 100);
                p->drawSocket();
                p->drawDecoration();
            } });
			outPins.push_back(p);
		}
	}
