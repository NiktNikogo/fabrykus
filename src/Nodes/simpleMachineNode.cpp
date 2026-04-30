#include "simpleMachineNode.hpp"

#include <ImNodeFlow.h>
#include <format>
#include <string>
#include <algorithm>
#include <memory>

#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

SimpleMachineNode::SimpleMachineNode() : fuel(0.0), time(0.0), inPins(), outPins{}
{
}
SimpleMachineNode::SimpleMachineNode(size_t id) : id(id), fuel(1.0), time(1.0), inPins(), outPins{}
{
    ins = {{1, "Iron ore"}};
    outs = {{2, "Iron ingot"}};
    setTitle(NodeFactory::getNameFromType(type).c_str());
    setStyle(ImFlow::NodeStyle::cyan());

    syncPins();
}

SimpleMachineNode::SimpleMachineNode(size_t id, double time, double fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
    : id(id),
      time(time),
      fuel(fuel),
      ins(ins),
      outs(outs)
{
    syncPins();
}

auto SimpleMachineNode::draw() -> void
{

    ImGui::Text("UID: 0x%lX", this->getUID());
    ImGui::Text("ID: %zd", this->getId());
    ImGui::Text("Optimal amount: %f", this->calcOptimalCount());
    ImGui::PushItemWidth(100.f);

    ImGui::Text("Time:");
    ImGui::InputDouble("##Time", &time);

    ImGui::Text("Fuel:");
    ImGui::InputDouble("##Fuel", &fuel);

    float eff = this->calcEfficiency();

    ImVec4 color = eff < 1.0f ? ImVec4(1, 0.5f, 0, 1) : ImVec4(0, 1, 0, 1);
    ImGui::Text("Status");
    ImGui::TextColored(color, "%.0f%% efficency", eff * 100.0f);

    if (eff < 1.0f)
    {
        ImGui::TextColored(ImVec4(1, 0.3f, 0.3f, 1), "Bottleneck");
    }

    update();
}

auto SimpleMachineNode::drawInspector() -> bool
{
    if (formatInputIngredients("Inputs:", "in", ins, this->getIns(), [this](uintptr_t uid)
                               { this->dropIN(uid); }))
    {
        return true;
    }
    if (formatInputIngredients("Output:", "out", outs, this->getOuts(), [this](uintptr_t uid)
                               { this->dropOUT(uid); }))
    {
        return true;
    }

    return false;
}

auto SimpleMachineNode::update() -> void
{
    for (size_t i = 0; i < ins.size(); i++)
    {
        auto p = inPin(i);
        if (!p)
            continue;
        if (p->isConnected())
        {
            auto linkPtr = p->getLink().lock();
            if (!linkPtr)
                continue;

            auto other = linkPtr->left();
            auto otherNode = dynamic_cast<SimpleMachineNode *>(other->getParent());

            if (otherNode)
            {
                size_t otherIdx = other->getUid();

                std::string myInputName = this->ins[i].name;
                std::string theirOutputName = otherNode->outs[otherIdx].name;

                if (myInputName != theirOutputName)
                {
                    p->deleteLink();
                }
            }
        }
    }
}

auto SimpleMachineNode::syncPins() -> void
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

        if (out->getUid() >= outNode->outs.size() || in->getUid() >= inNode->ins.size())
            return false;

        return outNode->outs[out->getUid()].name == inNode->ins[in->getUid()].name;
    };

    for (size_t i = 0; i < ins.size(); i++)
    {
        auto p = this->addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, LabelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < ins.size()) {
                Ingredient recived = getInVal<Ingredient>(i);
                double demand = this->ins[i].amount/this->time * this->calcOptimalCount();
                ImGui::Text("%s", this->ins[i].name.c_str());
                ImGui::TextDisabled("R: %.2f units/s ", ins[i].amount);
                ImGui::TextDisabled("I: %.2f units/s ", recived.amount);

                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }

    for (size_t i = 0; i < outs.size(); i++)
    {
        auto p = this->addOUT_uid<Ingredient>(i, " ")
                     ->behaviour([this, i]()
                                 {
                float eff = this->calcEfficiency();
                float count = this->calcOptimalCount();
                Ingredient result = this->outs[i];
                
                if(this->time > 0) {
                    result.amount = (result.amount/this->time) * eff * count;
                } else {
                    result.amount = 0;
                }
            
                return result; 
            });
        p->renderer([this, i](ImFlow::Pin *p)
                    {
            if (i < outs.size()) {
                float eff = this->calcEfficiency();
                float count = this->calcOptimalCount();
                Ingredient result = this->outs[i];
                ImGui::Text("%s", this->outs[i].name.c_str());
                ImGui::TextDisabled("R: %.2f units/s ", result.amount/this->time);
                ImGui::TextDisabled("O: %.2f units/s ", (result.amount/this->time) * eff * count);
                p->drawSocket();
                p->drawDecoration();
            } });

        outPins.push_back(p);
    }
}

const auto SimpleMachineNode::print() const -> void
{
    std::cout << "-----------\n";
    std::cout << std::format("UID: {}\n", this->getUID());
    std::cout << std::format("Fuel: {}\n", this->fuel);
    std::cout << std::format("Time: {}\n", this->time);
    std::cout << "ins: \n";
    for (const auto &in : ins)
    {
        std::cout << std::format("Name: {}| Amount: {}\n", in.name, in.amount);
    }
    std::cout << "outs: \n";
    for (const auto &out : outs)
    {
        std::cout << std::format("Name: {}| Amount: {}\n", out.name, out.amount);
    }
    std::cout << "-----------\n";
}

auto SimpleMachineNode::serialize() -> nlohmann::json
{
    nlohmann::json node;
    node["id"] = id;
    node["type"] = getNodeType();
    node["time"] = time;
    node["fuel"] = fuel;
    node["ins"] = ins;
    node["outs"] = outs;
    node["pos"] = nlohmann::json({{"x", getPos().x},
                                  {"y", getPos().y}});
    return node;
}

auto SimpleMachineNode::deserialize(nlohmann::json data) -> void
{

    setTitle(NodeFactory::getNameFromType(type).c_str());
    setStyle(ImFlow::NodeStyle::cyan());

    id = data["id"];
    fuel = data["fuel"];
    time = data["time"];
    ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
    setPos(pos);
    ins = data["ins"].get<std::vector<Ingredient>>();
    outs = data["outs"].get<std::vector<Ingredient>>();

    syncPins();
}

const auto SimpleMachineNode::getInPinIndex(ImFlow::Pin *pin) const -> size_t
{
    auto it = std::find(inPins.begin(), inPins.end(), pin);
    if (it == inPins.end())
        return 99999;
    return std::distance(inPins.begin(), it);
}

const auto SimpleMachineNode::getOutPinIndex(ImFlow::Pin *pin) const -> size_t
{
    auto it = std::find(outPins.begin(), outPins.end(), pin);
    if (it == outPins.end())
        return 99999;
    return std::distance(outPins.begin(), it);
}

auto SimpleMachineNode::formatInputIngredients(const char *category, const char *prefix,
                                               std::vector<Ingredient> &list,
                                               const std::vector<std::shared_ptr<ImFlow::Pin>> &pins, std::function<void(uintptr_t)> dropFunc,
                                               InspectorConfig config) -> bool
{

    auto totalWidth = ImGui::GetContentRegionAvail().x;
    float amountWidth = config.showAmmount ? totalWidth * 0.2f : 0.0f;
    float nameWidth = config.showAmmount ? totalWidth * 0.5f : totalWidth * 0.7f;

    ImGui::Text("%s", category);
    if (config.allowAdding)
    {
        ImGui::SameLine();

        if (ImGui::Button(std::format("+##Add{}", prefix).c_str()))
        {
            list.push_back({0, "New"});
            return true;
        }
    }

    for (auto i = 0; i < list.size(); i++)
    {
        ImGui::PushID(i);
        ImGui::Text("%d:", i);
        ImGui::SameLine();

        if (config.showAmmount)
        {
            ImGui::PushItemWidth(amountWidth);
            ImGui::InputDouble(std::format("##{}Amt{}", prefix, i).c_str(), &list[i].amount);
            ImGui::PopItemWidth();
            ImGui::SameLine();
        }

        ImGui::PushItemWidth(nameWidth);
        char buffer[SimpleMachineNode::TEXT_INPUT_MAX_LENGTH]{};
        snprintf(buffer, sizeof(buffer), "%s", list[i].name.c_str());
        if (ImGui::InputText(std::format("##{}Name{}", prefix, i).c_str(), buffer, sizeof(buffer)))
        {
            auto newName = std::string(buffer);
            if (config.sameName)
            {
                for (auto &in : list)
                {
                    in.name = newName;
                }
                for (auto &other : *config.keeps) {
                    other.name = newName;
                }
            }
            else
            {
                list[i].name = std::string(buffer);
            }
        }
        ImGui::PopItemWidth();

        if (config.allowDeleting)
        {
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));
            if (ImGui::Button(std::format("X##{}_Del_{}", prefix, i).c_str()))
            {
                dropFunc(pins[i]->getUid());
                list.erase(list.begin() + i);
                ImGui::PopStyleColor();
                ImGui::PopID();
                return true;
            }
            ImGui::PopStyleColor();
        }
        ImGui::PopID();
    }
    return false;
}

auto SimpleMachineNode::calcEfficiency() -> double
{
    float minSatisfaction = 1.0;

    if (this->time <= 0)
        return 0.0f;

    for (size_t i = 0; i < ins.size(); i++)
    {
        Ingredient recived = getInVal<Ingredient>(i);

        if (recived.name == ins[i].name)
        {

            double demand = ins[i].amount / this->time;

            if (demand <= 0)
                continue;

            float satisfaction = recived.amount / demand;
            minSatisfaction = std::min(satisfaction, minSatisfaction);
        }
        else
        {
            return 0.0;
        }
    }
    return minSatisfaction;
}

auto SimpleMachineNode::calcOptimalCount() -> double
{
    if (time <= 0 || ins.empty())
        return 0.0;

    double maxCount = 0.0;

    for (size_t i = 0; i < ins.size(); i++)
    {
        Ingredient recived = getInVal<Ingredient>(i);

        if (recived.name == ins[i].name)
        {

            double demand = ins[i].amount / this->time;

            if (demand <= 0)
                continue;

            double req = recived.amount / demand;
            maxCount = std::max(maxCount, req);
        }
        else
        {
            return 0.0;
        }
    }
    return maxCount;
}
