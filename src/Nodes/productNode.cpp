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
    setTitle(getTitle());
    setStyle(getColor());

    syncPins();
}

ProductNode::ProductNode(size_t id, std::vector<Ingredient> ins)
    :
    SimpleMachineNode(id, 1.0, 1.0, ins, {})
{
    type = NodeType::PRODUCT;
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

    for (size_t i = 0; i < getInList().size(); i++)
    {
        auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < getInList().size()) {
                ImGui::Text("%s", this->getInList()[i].name.c_str());
                ImGui::TextDisabled("I: %.2f units/s ", getInVal<Ingredient>(i).amount);
                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }

    for (size_t i = 0; i < getOutList().size(); i++)
    {
        auto p = this->addOUT_uid<Ingredient>(i, " ")->behaviour([this, i]() {
            return Ingredient{ this->getOutList()[i].amount / time, this->getOutList()[i].name };
        });
        
        p->renderer([this, i](ImFlow::Pin *p) {
            if (i < getOutList().size()) {
                ImGui::Text("%s", this->getOutList()[i].name.c_str());
                ImGui::TextDisabled("O: %.2f units/s ", getOutList()[i].amount/time);
                p->drawSocket();
                p->drawDecoration();
            } 
        });
        outPins.push_back(p);
    }
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
