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
	ImGui::Text("UID: 0x%lX", this->getUID());
	ImGui::Text("ID: %zd", this->getId());
	ImGui::PushItemWidth(100.f);
}

auto SplitterNode::drawInspector() -> bool
{
	if (formatInputIngredients("Inputs:", "in", getInList(), this->getIns(), [this](uintptr_t uid)
							   { this->dropIN(uid); }, {false, false, false, true, &getOutList()}))
	{
		return true;
	}
	if (formatInputIngredients("Output:", "out", getOutList(), this->getOuts(), [this](uintptr_t uid)
							   { this->dropOUT(uid); }, {true, true, true, true, &getInList()}))
	{
		return true;
	}

	return false;
}

auto SplitterNode::deserialize(nlohmann::json data) -> void
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
