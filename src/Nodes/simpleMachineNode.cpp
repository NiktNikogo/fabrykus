#include "simpleMachineNode.hpp"

#include <ImNodeFlow.h>
#include <format>
#include <string>
#include <algorithm>
#include <memory>

#include "Util/ingredient.hpp"
#include "Util/nodeFactory.hpp"

SimpleMachineNode::SimpleMachineNode() : fuel(1.0), time(1.0), inPins(), outPins{}, flow(Flow::FORWARD)
{
}
SimpleMachineNode::SimpleMachineNode(size_t id) : id(id), fuel(1.0), time(1.0), inPins(), outPins{}, flow(Flow::FORWARD)
{
    ins = {{1, "Iron ore"}};
    outs = {{2, "Iron ingot"}};
    setTitle(getTitle());
    setStyle(getColor());

    syncPins();
}

SimpleMachineNode::SimpleMachineNode(size_t id, double time, double fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs)
    : id(id),
      time(time),
      fuel(fuel),
      ins(ins),
      outs(outs),
      flow(Flow::FORWARD)
{
    syncPins();
}

auto SimpleMachineNode::draw() -> void
{

    ImGui::Text("UID: 0x%lX", getUID());
    ImGui::Text("ID: %zd", getId());
    ImGui::Text("Optimal amount: %f", calcOptimalCount());
    ImGui::PushItemWidth(100.f);

    ImGui::Text("Time:");
    ImGui::InputDouble("##Time", &time);

    ImGui::Text("Fuel:");
    ImGui::InputDouble("##Fuel", &fuel);

    float eff = calcEfficiency();

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
    if (formatInputIngredients("Inputs:", "in", getInList(), getIns(), [this](uintptr_t uid)
                               { dropIN(uid); }))
    {
        return true;
    }
    if (formatInputIngredients("Output:", "out", getOutList(), getOuts(), [this](uintptr_t uid)
                               { dropOUT(uid); }))
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

                std::string myInputName = getInList()[i].name;
                std::string theirOutputName = otherNode->getOutList()[otherIdx].name;

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
    for (auto &p : getIns())
        inUids.push_back(p->getUid());
    for (auto &p : getOuts())
        outUids.push_back(p->getUid());
    for (auto id : inUids)
        dropIN(id);
    for (auto id : outUids)
        dropOUT(id);
    inPins.clear();
    outPins.clear();
    
    for (size_t i = 0; i < getInList().size(); i++)
    {
        auto p = addIN_uid<Ingredient>(i, " ", Ingredient{0, ""}, labelMatchFilter)->renderer([this, i](ImFlow::Pin *p)
                                                                                                    {
            if (i < getInList().size()) {
                Ingredient recived = getInVal<Ingredient>(i);
                double demand = getInList()[i].asDouble()/time * calcOptimalCount();
                ImGui::Text("%s", getInList()[i].name.c_str());
                ImGui::TextDisabled("R: %.2f units/s ", getInList()[i].asDouble());
                ImGui::TextDisabled("I: %.2f units/s ", recived.asDouble());

                ImGui::SameLine();
                p->drawSocket();
                p->drawDecoration();
            } });
        inPins.push_back(p);
    }

    for (size_t i = 0; i < getOutList().size(); i++)
    {
        auto p = addOUT_uid<Ingredient>(i, " ")
                     ->behaviour([this, i]()
                                 {
                float eff = calcEfficiency();
                float count = calcOptimalCount();
                Ingredient result = getOutList()[i];
                
                if(time > 0) {
                    result.fromDouble((result.asDouble()/time) * eff * count);
                } else {
                    result.amount = 0;
                }
            
                return result; });
        p->renderer([this, i](ImFlow::Pin *p)
                    {
            if (i < getOutList().size()) {
                float eff = calcEfficiency();
                float count = calcOptimalCount();
                Ingredient result = getOutList()[i];
                ImGui::Text("%s", getOutList()[i].name.c_str());
                ImGui::TextDisabled("R: %.2f units/s ", result.asDouble()/time);
                ImGui::TextDisabled("O: %.2f units/s ", (result.asDouble()/time) * eff * count);
                p->drawSocket();
                p->drawDecoration();
            } });

        outPins.push_back(p);
    }
}

const auto SimpleMachineNode::print() const -> void
{
    std::cout << "-----------\n";
    std::cout << std::format("UID: {}\n", getUID());
    std::cout << std::format("Fuel: {}\n", fuel);
    std::cout << std::format("Time: {}\n", time);
    std::cout << "ins: \n";
    for (const auto &in : ins)
    {
        std::cout << std::format("Name: {}| Amount: {}\n", in.name, in.asDouble());
    }
    std::cout << "outs: \n";
    for (const auto &out : outs)
    {
        std::cout << std::format("Name: {}| Amount: {}\n", out.name, out.asDouble());
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
    node["flow"] = isReverseFlow;                                  
    return node;
}

auto SimpleMachineNode::deserialize(nlohmann::json data) -> void
{

    id = data["id"];
    fuel = data["fuel"];
    time = data["time"];
    ImVec2 pos = {data["pos"]["x"], data["pos"]["y"]};
    setPos(pos);
    ins = data["ins"].get<std::vector<Ingredient>>();
    outs = data["outs"].get<std::vector<Ingredient>>();
    if(data.contains("flow")) {
        isReverseFlow = data["flow"].get<bool>();
    } else {
        isReverseFlow = false;
    }

    setTitle(getTitle());
    setStyle(getColor());
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

const auto SimpleMachineNode::getTitle() -> std::string
{
    std::string base = NodeFactory::getNameFromType(type);
    if(isReverseFlow) {
        base += "^-1";
    }
    return base.c_str();
}

const auto SimpleMachineNode::getColor() -> std::shared_ptr<ImFlow::NodeStyle>
{
    if(isReverseFlow) {
        return std::make_shared<ImFlow::NodeStyle>(
            IM_COL32(255, 127, 80, 255),  //cyan
            ImColor(233, 241, 244, 255),
            6.5f
        );
    } else {
	    return ImFlow::NodeStyle::cyan();
    }
}

auto SimpleMachineNode::labelMatchFilter(ImFlow::Pin *out, ImFlow::Pin *in) -> bool
{
	 {
        auto *outNode = dynamic_cast<SimpleMachineNode *>(out->getParent());
        auto *inNode = dynamic_cast<SimpleMachineNode *>(in->getParent());
        if (!outNode || !inNode)
            return false;
        if(inNode->getIsReversed() != outNode->getIsReversed()) {
            return false;
        }
        
        if (out->getUid() >= outNode->getOutList().size() || in->getUid() >= inNode->getInList().size())
            return false;
        
        return outNode->getOutList()[out->getUid()].name == inNode->getInList()[in->getUid()].name;
    };
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
    Rational currAmount = Rational(0, 1);
    for(const auto& ing : list) {
        currAmount += ing.amount;
    }
    if(config.capInputValues && currAmount != config.cappedValue) {
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"Input values don't sum up to %.2f", boost::rational_cast<double>(config.cappedValue));
    }
    for (auto i = 0; i < list.size(); i++)
    {
        ImGui::PushID(i);
        ImGui::Text("%d:", i);
        ImGui::SameLine();
        if (config.showAmmount)
        {
            double tempVal = list[i].asDouble();
            ImGui::PushItemWidth(amountWidth);
            if(ImGui::InputDouble(std::format("##{}Amt{}", prefix, i).c_str(), &tempVal)) {
                list[i].fromDouble(tempVal);
            }
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
                for (auto &other : *config.keeps)
                {
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

    if (time <= 0)
        return 0.0f;

    auto ingredients = getInList();
    for (size_t i = 0; i < ingredients.size(); i++)
    {
        Ingredient recived = getInVal<Ingredient>(i);

        if (recived.name == ingredients[i].name)
        {

            double demand = ingredients[i].asDouble() / time;

            if (demand <= 0)
                continue;

            float satisfaction = recived.asDouble() / demand;
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
    auto ingredients = getInList();
    if (time <= 0 || ingredients.empty())
        return 0.0;

    double maxCount = 0.0;

    for (size_t i = 0; i < ingredients.size(); i++)
    {
        Ingredient recived = getInVal<Ingredient>(i);

        if (recived.name == ingredients[i].name)
        {

            double demand = ingredients[i].asDouble() / time;

            if (demand <= 0)
                continue;

            double req = recived.asDouble() / demand;
            maxCount = std::max(maxCount, req);
        }
        else
        {
            return 0.0;
        }
    }
    return maxCount;
}
