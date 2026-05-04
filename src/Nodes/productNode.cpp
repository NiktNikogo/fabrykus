#include "productNode.hpp"
#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

ProductNode::ProductNode() : TerminalNode()
{
    type = NodeType::PRODUCT;
    mode = Mode::PRODUCT;
}

ProductNode::ProductNode(size_t id) : TerminalNode(id)
{
    type = NodeType::PRODUCT;
    mode = Mode::PRODUCT;
    ins.clear();
    outs.clear();
    ins.push_back({2, "Iron ingot"});
    time = 1;
    setTitle(getTitle());
    setStyle(getColor());

    syncPins();
}

ProductNode::ProductNode(size_t id, std::vector<Ingredient> ins)
    :
    TerminalNode(id, {}, outs)
{
    type = NodeType::PRODUCT;
    mode = Mode::PRODUCT;
    setTitle(getTitle());
    setStyle(getColor());
    syncPins();
}

auto ProductNode::draw() -> void
{
    ImGui::Text("UID: 0x%lX", this->getUID());
    ImGui::Text("ID: %zd", this->getId());
    ImGui::PushItemWidth(100.f);

    if(isReverseFlow) {
        ImGui::Text("Time:");
        ImGui::SameLine();
        ImGui::InputDouble("##Time", &time);
    }
}

auto ProductNode::update() -> void
{
}

auto ProductNode::drawInspector() -> bool
{
    if(!isReverseFlow) {
        if (formatInputIngredients("Input:", "in", getInList(), this->getIns(), [this](uintptr_t uid)
                                { this->dropIN(uid); }))
        {
            return true;
        } 
    } else {
        if (formatInputIngredients("Output:", "out", getOutList(), this->getOuts(), [this](uintptr_t uid)
                                { this->dropOUT(uid); }))
        {
            return true;
        }
    }
    return false;
}

auto ProductNode::deserialize(nlohmann::json data) -> void 
{  
    setTitle(getTitle());
    setStyle(getColor());

    id = data["id"];
    ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
    setPos(pos);
    ins = data["ins"].get<std::vector<Ingredient>>();
    syncPins();
}

const auto ProductNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle> 
{
    if(isReverseFlow) {
        return std::make_shared<ImFlow::NodeStyle>(
            IM_COL32(34, 139, 34, 255),  //dark green
            ImColor(233, 241, 244, 255),
            6.5f
        );
    } else {
	    return ImFlow::NodeStyle::red();
    }
}
