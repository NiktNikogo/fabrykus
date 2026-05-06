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

    update();
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

const auto IngredientNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle>
{  
    if(isReverseFlow) {
        return std::make_shared<ImFlow::NodeStyle>(
            IM_COL32(34, 139, 34, 255),  //dark green
            ImColor(233, 241, 244, 255),
            6.5f
        );
    } else {
	    return ImFlow::NodeStyle::green();
    }
}
