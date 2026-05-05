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
	if (formatInputIngredients("Inputs:", "in", getInList(), this->getIns(), [this](uintptr_t uid)
							   { this->dropIN(uid); }, {false, true, true, true, &getOutList()}))
	{
		return true;
	}
	if (formatInputIngredients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
							   { this->dropOUT(uid); }, {false, false, false, true, &getInList()}))
	{
		return true;
	}

	return false;
}

const auto MergerNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle>
{
	if (isReverseFlow) {
		return std::make_shared<ImFlow::NodeStyle>(
			IM_COL32(0, 127, 255, 255), //azure blue
			ImColor(255, 255, 255, 255),
			6.5f 
		); 
	} else {
		return std::make_shared<ImFlow::NodeStyle>(
			IM_COL32(204, 85, 0, 255),  //orange
			ImColor(255, 255, 255, 255),
			6.5f 
		); 
	}
}
