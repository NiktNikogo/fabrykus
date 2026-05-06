#include "splitterNode.hpp"
#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

SplitterNode::SplitterNode() : DistributorNode()
{
	type = NodeType::SPLITTER;
	mode = Mode::SPLITTER;
}

SplitterNode::SplitterNode(size_t id) : DistributorNode(id)
{
	mode = Mode::SPLITTER;
	type = NodeType::SPLITTER;
	ins.clear();
	outs.clear();
	ins.push_back({1, "Iron ore"});
	outs.push_back({0.5, "Iron ore"});
	outs.push_back({0.5, "Iron ore"});
	
    setTitle(getTitle());
    setStyle(getColor());

	syncPins();
}

SplitterNode::SplitterNode(size_t id, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
	: DistributorNode(id, ins, outs)
{
	mode = Mode::SPLITTER;
	type = NodeType::SPLITTER;
	
    setTitle(getTitle());
    setStyle(getColor());

	inAmount = 0.0;
    totalIntake = 0.0;
	syncPins();
}

auto SplitterNode::draw() -> void
{
	ImGui::Text("UID: 0x%lX", this->getUID());
	ImGui::Text("ID: %zd", this->getId());
	ImGui::PushItemWidth(100.f);


	bool isMerging = (mode == Mode::MERGER && !isReverseFlow) || (mode == Mode::SPLITTER && isReverseFlow);
	bool isSplitting = !isMerging;

	float currentTotal = 0.0f;
	for (size_t j = 0; j < getInList().size(); j++) {
		currentTotal += getInVal<Ingredient>(j).asDouble();
	}
	totalIntake = currentTotal;

	if (isSplitting && !getInList().empty()) {
		inAmount = getInVal<Ingredient>(0).asDouble();
	}


	update();
}

const auto SplitterNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle>
{
	if (isReverseFlow) {
		return std::make_shared<ImFlow::NodeStyle>(
           	IM_COL32(218, 165, 32, 255),  //gold
            ImColor(233, 241, 244, 255),
            6.5f
        );
	} else {
		return std::make_shared<ImFlow::NodeStyle>(
            IM_COL32(128, 0, 128, 255),  //purple
            ImColor(233, 241, 244, 255),
            6.5f
        );
	}
}
