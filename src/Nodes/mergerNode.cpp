#include "mergerNode.hpp"

#include <numeric>
#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

MergerNode::MergerNode() : SimpleMachineNode()
{
	type = NodeType::MERGER;
}

MergerNode::MergerNode(size_t id) : SimpleMachineNode(id)
{
	type = NodeType::MERGER;
	ins.clear();
	outs.clear();
	ins.push_back({1, "Iron ingot"});
	ins.push_back({1, "Iron ingot"});
	outs.push_back({1, "Iron ingot"});
	setTitle(NodeFactory::getNameFromType(type).c_str());
	setStyle(orange());

	syncPins();
}

MergerNode::MergerNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
	: SimpleMachineNode(id, 1.0, 1.0, ins, outs)
{
	type = NodeType::MERGER;
	setTitle(NodeFactory::getNameFromType(type).c_str());
	setStyle(orange());

	syncPins();
}

auto MergerNode::draw() -> void
{
	ImGui::Text("UID: 0x%lX", this->getUID());
	ImGui::Text("ID: %zd", this->getId());
	ImGui::PushItemWidth(100.f);
}

auto MergerNode::update() -> void
{
}

auto MergerNode::syncPins() -> void
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
	for (size_t i = 0; i < ins.size(); i++)
	{
		auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer(
			[this, i](ImFlow::Pin *p) {
				if (i < ins.size()) {
					auto intake = getInVal<Ingredient>(i).amount/totalIntake;
					ins[i].amount = intake;
					ImGui::Text("%s", this->ins[i].name.c_str());
					ImGui::TextDisabled("I: %.2f%%", ins[i].amount);
					ImGui::SameLine();
					p->drawSocket();
					p->drawDecoration();
				} 
			});
		inPins.push_back(p);
	}
	for (size_t i = 0; i < outs.size(); i++)
	{

		auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]()
																 { 
			this->totalIntake = 0.0f;
			for(size_t j = 0; j < ins.size(); j++) {
				auto  ing = getInVal<Ingredient>(j);
				totalIntake += getInVal<Ingredient>(j).amount;
			}
			return Ingredient{totalIntake, this->outs[i].name}; });

		p->renderer([this, i](ImFlow::Pin *p)
					{
            if (i < outs.size()) {
                ImGui::Text("%s", this->outs[i].name.c_str());
				ImGui::TextDisabled("O: %.2f%%", totalIntake);
                p->drawSocket();
                p->drawDecoration();
            } });
		outPins.push_back(p);
	}
}

auto MergerNode::drawInspector() -> bool
{
	if (formatInputIngredients("Inputs:", "in", ins, this->getIns(), [this](uintptr_t uid)
							   { this->dropIN(uid); }, {false, true, true, true, &outs}))
	{
		return true;
	}
	if (formatInputIngredients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
							   { this->dropOUT(uid); }, {false, false, false, true, &ins}))
	{
		return true;
	}

	return false;
}

auto MergerNode::deserialize(nlohmann::json data) -> void
{
	setTitle(NodeFactory::getNameFromType(type).c_str());
	setStyle(orange());

	id = data["id"];
	fuel = data["fuel"];
	time = data["time"];
	ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
	setPos(pos);
	ins = data["ins"].get<std::vector<Ingredient>>();
	outs = data["outs"].get<std::vector<Ingredient>>();

	syncPins();
}
