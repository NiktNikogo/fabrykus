#include "ingredientNode.hpp"
#include "Util/nodeFactory.hpp"


IngredientNode::IngredientNode() : TerminalNode()
{
    type = NodeType::INGREDIENT;
    mode = Mode::INGREDIENT;
}

IngredientNode::IngredientNode(size_t id): TerminalNode(id)
{
    type = NodeType::INGREDIENT;
    mode = Mode::INGREDIENT;
    ins.clear();
    outs.clear();
    outs.push_back({1, "Iron ore"});
    setTitle(getTitle());
    setStyle(getColor());
    syncPins();
}

IngredientNode::IngredientNode(size_t id, double time, std::vector<Ingredient> outs)
    :
    TerminalNode(id, {}, outs)
{
    type = NodeType::INGREDIENT;
    mode = Mode::INGREDIENT;
    setTitle(getTitle());
    setStyle(getColor());
    syncPins();
}

auto IngredientNode::draw() -> void
{
    ImGui::Text("UID: 0x%lX", this->getUID());
    ImGui::Text("ID: %zd", this->getId());
    
    ImGui::PushItemWidth(100.f);

    if(!isReverseFlow) {
        ImGui::Text("Time:");
        ImGui::SameLine();
        ImGui::InputDouble("##Time", &time);
    }
}

auto IngredientNode::update() -> void
{
}

auto IngredientNode::drawInspector() -> bool 
{   
    if(!isReverseFlow) {
        if (formatInputIngredients("Output:", "out", getOutList(), this->getOuts(), [this](uintptr_t uid)
                                { this->dropOUT(uid); }))
        {
            return true;
        }
    } else {
        if (formatInputIngredients("Input:", "in", getInList(), this->getIns(), [this](uintptr_t uid)
                               { this->dropIN(uid); }))
        {
            return true;
        } 
    }
    return false;
}

auto IngredientNode::deserialize(nlohmann::json data) -> void
{
    setTitle(getTitle());
    setStyle(getColor());

    id = data["id"];
    time = data["time"];
    ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
    setPos(pos);
    outs = data["outs"].get<std::vector<Ingredient>>();
 
    syncPins();
}

const auto IngredientNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle>
{  
    if(isReverseFlow) {
        return std::make_shared<ImFlow::NodeStyle>(
            IM_COL32(225, 100, 100, 255),  //rose
            ImColor(233, 241, 244, 255),
            6.5f
        );
    } else {
	    return ImFlow::NodeStyle::green();
    }
}
