#include "productNode.hpp"
#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

ProductNode::ProductNode() : SimpleMachineNode()
{
    type = NodeType::PRODUCT;
}

ProductNode::ProductNode(size_t id) : SimpleMachineNode(id)
{
    type = NodeType::PRODUCT;
    ins.clear();
    outs.clear();
    ins.push_back({2, "Iron ingot"});
    time = 1;
    setTitle(NodeFactory::getNameFromType(type).c_str());
    setStyle(ImFlow::NodeStyle::red());

    syncPins();
}

ProductNode::ProductNode(size_t id, std::vector<Ingredient> ins)
    :
    SimpleMachineNode(id, 1.0, 1.0, ins, {})
{
    type = NodeType::PRODUCT;
    setTitle(NodeFactory::getNameFromType(type).c_str());
    setStyle(ImFlow::NodeStyle::red());
    syncPins();
}

auto ProductNode::draw() -> void
{
    ImGui::Text("UID: 0x%lX", this->getUID());
    ImGui::Text("ID: %zd", this->getId());
    ImGui::PushItemWidth(100.f);

    for (size_t i = 0; i < inPins.size(); i++)
    {
        if (inPins[i])
        {
            Ingredient curr = getInVal<Ingredient>(inPins[i]->getUid());
            ImGui::Text("%s: %.2f units/sec", curr.name.c_str(), (float)curr.amount);
        }
    }
}

auto ProductNode::update() -> void
{
}

auto ProductNode::drawInspector() -> bool
{
    if (formatInputIngredients("Input:", "in", ins, this->getIns(), [this](uintptr_t uid)
                               { this->dropIN(uid); }))
    {
        return true;
    } 
    return false;
}

auto ProductNode::deserialize(nlohmann::json data) -> void 
{
    setTitle("Storage");
    setStyle(ImFlow::NodeStyle::red());

    id = data["id"];
    ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
    setPos(pos);
    ins = data["ins"].get<std::vector<Ingredient>>();
    syncPins();
}
