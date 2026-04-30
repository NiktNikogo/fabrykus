#include "splitterNode.hpp"
#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

SplitterNode::SplitterNode() : SimpleMachineNode()
{
	type = NodeType::SPLITTER;
}

SplitterNode::SplitterNode(size_t id) : SimpleMachineNode(id)
{
	type = NodeType::SPLITTER;
	ins.clear();
	outs.clear();
	ins.push_back({1, "Iron ore"});
	outs.push_back({0.5, "Iron ore"});
	outs.push_back({0.5, "Iron ore"});
	setTitle(NodeFactory::getNameFromType(type).c_str());
	setStyle(purple());

	syncPins();
}

SplitterNode::SplitterNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
	: SimpleMachineNode(id, 1.0, 1.0, ins, outs)
{
	type = NodeType::SPLITTER;
	setTitle(NodeFactory::getNameFromType(type).c_str());
	setStyle(purple());

	syncPins();
}

auto SplitterNode::draw() -> void
{
	ImGui::Text("UID: 0x%lX", this->getUID());
	ImGui::Text("ID: %zd", this->getId());
	ImGui::PushItemWidth(100.f);
}

auto SplitterNode::update() -> void
{
}

auto SplitterNode::syncPins() -> void
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
		auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
																									{
			if (i < ins.size()) {
				Ingredient recived = getInVal<Ingredient>(i);
				this->inAmount = recived.amount;
				
				ImGui::Text("%s", this->ins[i].name.c_str());
				ImGui::TextDisabled("I: %.2f", inAmount);
				ImGui::SameLine();
				p->drawSocket();
				p->drawDecoration();
			} });
		inPins.push_back(p);
	}

	for (size_t i = 0; i < outs.size(); i++)
	{
		auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]()
																 { return Ingredient{this->outs[i].amount * this->inAmount, this->outs[i].name}; });

		p->renderer([this, i](ImFlow::Pin *p)
					{
			if (i < outs.size()) {
				ImGui::Text("%s", this->outs[i].name.c_str());
				ImGui::TextDisabled("O: %.2f%%", this->outs[i].amount);
				p->drawSocket();
				p->drawDecoration();
			} });
		outPins.push_back(p);
	}
}

auto SplitterNode::drawInspector() -> bool
{
	if (formatInputIngredients("Inputs:", "in", ins, this->getIns(), [this](uintptr_t uid)
							   { this->dropIN(uid); }, {false, false, false, true, &outs}))
	{
		return true;
	}
	if (formatInputIngredients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
							   { this->dropOUT(uid); }, {true, true, true, true, &ins}))
	{
		return true;
	}

	return false;
}

auto SplitterNode::deserialize(nlohmann::json data) -> void
{
	setTitle(NodeFactory::getNameFromType(type).c_str());
	setStyle(purple());

	id = data["id"];
	fuel = data["fuel"];
	time = data["time"];
	ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
	setPos(pos);
	ins = data["ins"].get<std::vector<Ingredient>>();
	outs = data["outs"].get<std::vector<Ingredient>>();

	syncPins();
}
