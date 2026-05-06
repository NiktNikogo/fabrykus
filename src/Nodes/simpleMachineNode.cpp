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
    ImGui::Text("Optimal amount: %d", boost::rational_cast<int>(calcOptimalCount()));
    ImGui::PushItemWidth(100.f);

    ImGui::Text("Time:");
    ImGui::InputDouble("##Time", &time);

    ImGui::Text("Fuel:");
    ImGui::InputDouble("##Fuel", &fuel);
  
    auto eff = calcBottleneck();
    ImVec4 color = eff < Rational(1) ? ImVec4(1, 0.5f, 0, 1) : ImVec4(0, 1, 0, 1);
    ImGui::Text("Status");
    ImGui::TextColored(color, "%.0f%% efficency", boost::rational_cast<double>(eff) * 100.0f);

    if (eff < Rational(1))
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
    for (size_t i = 0; i < getInList().size(); i++)
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
                auto count = calcOptimalCount();
                auto demand = getInList()[i].amount * count / Ingredient::makeFromDouble(time);
                ImGui::Text("%s", getInList()[i].name.c_str());
                ImGui::TextDisabled("R: %.2f units/s ", getInList()[i].asDouble());
                ImGui::TextDisabled("I: %.2f units/s ", recived.asDouble());
                ImGui::TextDisabled("T: %.2f units/s ", boost::rational_cast<double>(getInList()[i].amount * count));
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
                auto ratio = calcSatisfation();
                auto count = calcOptimalCount();
                auto througtput = ratio > count ? count : ratio;
                Ingredient result = getOutList()[i];
                auto output = result.amount / Ingredient::makeFromDouble(time) * througtput;
                if(time > 0) {
                    result.amount = output;
                } else {
                    result.amount = 0;
                }
            
                return result; });
        p->renderer([this, i](ImFlow::Pin *p)
                    {
            if (i < getOutList().size()) {
                auto ratio = calcSatisfation();
                auto count = calcOptimalCount();
                auto througtput = ratio > count ? count : ratio;
                Ingredient result = getOutList()[i];
                auto output = result.amount / Ingredient::makeFromDouble(time) * througtput;
                ImGui::Text("%s", getOutList()[i].name.c_str());
                ImGui::TextDisabled("R: %.2f units/s ", result.asDouble()/time);
                ImGui::TextDisabled("O: %.2f units/s ", boost::rational_cast<double>(output));
                ImGui::TextDisabled("T: %.2f units/s ", boost::rational_cast<double>(getOutList()[i].amount * count));
                p->drawSocket();
                p->drawDecoration();
            } });

        outPins.push_back(p);
    }
}

auto SimpleMachineNode::reverseFlow() -> void
{
    isReverseFlow = !isReverseFlow;
    setTitle(getTitle());
    setStyle(getColor());
    if(time == 0.0f) {
        time = 1;
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

const auto SimpleMachineNode::updateRecived() -> void
{
	// recived.clear();
    // for (size_t i = 0; i < getInList().size(); i++) {
    //     auto temp = getInVal<Ingredient>(i);
    //     recived.push_back(getInVal<Ingredient>(i));
    // }
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

auto SimpleMachineNode::calcSatisfation() -> Rational
{
    Rational minSatisfaction(INT_MAX, 1);

    if (time <= 0)
        return Rational(0);

    auto ingredients = getInList();
    for (size_t i = 0; i < ingredients.size(); i++)
    {
        Ingredient recived = getInVal<Ingredient>(i);

        if (recived.name == ingredients[i].name)
        {
            auto demand = ingredients[i].amount;
            if(time == 0.0f) {
                demand = Rational(0);
            } else {
               demand /= Ingredient::makeFromDouble(time);
            }

            if (demand <= 0)
                continue;

            auto satisfaction = recived.amount / demand;
            if(minSatisfaction > satisfaction) {
                minSatisfaction = satisfaction;
            }
        }
        else
        {
            return Rational(0);
        }
    }
    return minSatisfaction;
}

auto SimpleMachineNode::calcEfficiency() -> Rational
{
    auto sat = calcSatisfation();
	return sat < Rational(1) ? sat : Rational(1);
}

auto SimpleMachineNode::calcOptimalCount() -> Rational
{
    auto ingredients = getInList();
    if (time <= 0 || ingredients.empty())
        return Rational(0);

    Rational maxCount(0);

    for (size_t i = 0; i < ingredients.size(); i++)
    {
        Ingredient recived = getInVal<Ingredient>(i);

        if (recived.name == ingredients[i].name)
        {

            auto demand = ingredients[i].amount / Ingredient::makeFromDouble(time);

            if (demand <= 0)
                continue;

            auto req = recived.amount / demand;
            if(req > maxCount) {
                maxCount = req;
            }
        }
        else
        {
            return Rational(0);
        }
    }
    auto rounded = (long long)std::ceil(boost::rational_cast<double>(maxCount));
    maxCount = Rational(rounded);

    return maxCount;
}

auto SimpleMachineNode::calcBottleneck() -> Rational
{
    auto count = calcOptimalCount();
    auto sat = calcSatisfation();
    if(count == Rational(0) || sat == Rational(0)) {
        count = Rational(1);
    }
    sat = sat / count;
    return sat;
}
