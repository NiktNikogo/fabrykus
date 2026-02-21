#include "productNode.hpp"
#include "Util/ingredient.hpp"

ProductNode::ProductNode() : SimpleMachineNode()
{
 
}

ProductNode::ProductNode(size_t id) : SimpleMachineNode(id)
{
    ins.clear();
    outs.clear();
    ins.push_back({2, "Iron ingot"});
    time = 1;
    setTitle("Storage");
    setStyle(ImFlow::NodeStyle::red());

    syncPins();
}

ProductNode::ProductNode(size_t id, std::vector<Ingredient> ins)
    :
    SimpleMachineNode(id, 1.0, 1.0, ins, {})
{
    setTitle("Storage");
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

auto ProductNode::syncPins() -> void
{
    std::vector<uintptr_t> inUids, outUids;
    for (auto &p : this->getIns())
        inUids.push_back(p->getUid());
    for (auto &p : this->getOuts())
        outUids.push_back(p->getUid());
    for (auto id : inUids)
        this->dropIN(id);
    for (auto id : outUids)
        this->dropOUT(id);
    inPins.clear();
    outPins.clear();

    auto LabelMatchFilter = [this](ImFlow::Pin *out, ImFlow::Pin *in) -> bool
    {
        auto *outNode = dynamic_cast<SimpleMachineNode *>(out->getParent());
        auto *inNode = dynamic_cast<SimpleMachineNode *>(in->getParent());
        if (!outNode || !inNode)
            return false;

        if (out->getUid() >= outNode->getOutList().size() || in->getUid() >= inNode->getInList().size())
            return false;

        return outNode->getOutList()[out->getUid()].name == inNode->getInList()[in->getUid()].name;
    };

    for (size_t i = 0; i < ins.size(); i++)
    {
        auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < ins.size()) {
                ImGui::Text("%s", this->ins[i].name.c_str());
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }
}

auto ProductNode::drawInspector() -> void
{
    if (formatInputIngredients("Input:", "in", ins, this->getIns(), [this](uintptr_t uid)
                               { this->dropIN(uid); }))
    {
        return;
    }
}

const auto ProductNode::getNodeType() const -> NodeType 
{
    return NodeType::PRODUCT;
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
