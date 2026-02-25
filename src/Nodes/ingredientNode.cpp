#include "ingredientNode.hpp"
#include "Util/nodeFactory.hpp"


IngredientNode::IngredientNode() : SimpleMachineNode()
{
    type = NodeType::INGREDIENT;
}

IngredientNode::IngredientNode(size_t id): SimpleMachineNode(id)
{
    type = NodeType::INGREDIENT;
    ins.clear();
    outs.clear();
    outs.push_back({1, "Iron ore"});
    setTitle(NodeFactory::getNameFromType(type).c_str());
    setStyle(ImFlow::NodeStyle::green());
    syncPins();
}

IngredientNode::IngredientNode(size_t id, double time, std::vector<Ingredient> outs)
    :
    SimpleMachineNode(id, time, 1.0, {}, outs)
{
    type = NodeType::INGREDIENT;
    setTitle(NodeFactory::getNameFromType(type).c_str());
    setStyle(ImFlow::NodeStyle::green());
    syncPins();
}

auto IngredientNode::draw() -> void
{
    ImGui::Text("UID: 0x%lX", this->getUID());
    ImGui::Text("ID: %zd", this->getId());
    
    ImGui::PushItemWidth(100.f);

    ImGui::Text("Time:");
    ImGui::SameLine();
    ImGui::InputDouble("##Time", &time);
}

auto IngredientNode::update() -> void
{
}



auto IngredientNode::drawInspector() -> bool 
{   
    if (formatInputIngredients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
                               { this->dropOUT(uid); }))
    {
        return true;
    }
    return false;
}

auto IngredientNode::deserialize(nlohmann::json data) -> void
{
    setTitle("Source");
    setStyle(ImFlow::NodeStyle::green());

    id = data["id"];
    time = data["time"];
    ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
    setPos(pos);
    outs = data["outs"].get<std::vector<Ingredient>>();
 
    syncPins();
}
