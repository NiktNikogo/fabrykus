#include "mergerNode.hpp"

#include <numeric>
#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

MergerNode::MergerNode() : DistributorNode()
{
	type = NodeType::MERGER;
	mode = Mode::MERGER;
}

MergerNode::MergerNode(size_t id) : DistributorNode(id)
{
	type = NodeType::MERGER;
	mode = Mode::MERGER;
	ins.clear();
	outs.clear();
	ins.push_back({1, "Iron ingot"});
	ins.push_back({1, "Iron ingot"});
	outs.push_back({1, "Iron ingot"});
	
    setTitle(getTitle());
    setStyle(getColor());

	syncPins();
}

MergerNode::MergerNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
	: DistributorNode(id, ins, outs)
{
	type = NodeType::MERGER;
	mode = Mode::MERGER;
    setTitle(getTitle());
    setStyle(getColor());

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
    setTitle(getTitle());
    setStyle(getColor());

	id = data["id"];
	fuel = data["fuel"];
	time = data["time"];
	ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
	setPos(pos);
	ins = data["ins"].get<std::vector<Ingredient>>();
	outs = data["outs"].get<std::vector<Ingredient>>();

	syncPins();
}

const auto MergerNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle>
{
	return orange();
}
