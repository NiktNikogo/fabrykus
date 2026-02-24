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

auto IngredientNode::syncPins() -> void
{
    std::vector<uintptr_t> inUids, outUids;
    for(auto& p: this->getIns()) inUids.push_back(p->getUid());
    for(auto& p: this->getOuts()) outUids.push_back(p->getUid());
    for(auto id : inUids) this->dropIN(id);
    for(auto id : outUids) this->dropOUT(id);
    inPins.clear();
    outPins.clear();

    for (size_t i = 0; i < outs.size(); i++)
    {
        auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() {
            return Ingredient{ this->outs[i].amount / time, this->outs[i].name };
        });
        
        p->renderer([this, i](ImFlow::Pin *p) {
            if (i < outs.size()) {
                ImGui::Text("%s", this->outs[i].name.c_str());
                p->drawSocket();
                p->drawDecoration();
            } 
        });
        outPins.push_back(p);
    }
}

auto IngredientNode::drawInspector() -> void 
{   
    if (formatInputIngredients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
                               { this->dropOUT(uid); }))
    {
        return;
    }
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
