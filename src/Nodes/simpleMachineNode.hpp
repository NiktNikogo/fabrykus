#pragma once

#include <ImNodeFlow.h>
#include <memory>
#include <nlohmann/json.hpp>

#include "Util/ingredient.hpp"
#include "Util/nodeTypes.hpp"
#include "Util/inspectorConfig.hpp"

class SimpleMachineNode : public ImFlow::BaseNode
{

protected:
    size_t id = -1;
    double time;
    double fuel;
    std::vector<Ingredient> ins;
    std::vector<Ingredient> outs;
    std::vector<ImFlow::Pin *> inPins;
    std::vector<ImFlow::Pin *> outPins;
    bool isReverseFlow = false;

public:
    enum Flow {
        FORWARD,
        REVERSE
    };

public:
    static const size_t TEXT_INPUT_MAX_LENGTH = 128;
    NodeType type = NodeType::MACHINE;
    Flow flow;
protected:
    auto formatInputIngredients(const char *category, const char *prefix, std::vector<Ingredient> &list,
                                const std::vector<std::shared_ptr<ImFlow::Pin>> &pins, std::function<void(uintptr_t)> dropFunc,
                                InspectorConfig config = {true, true, true, false, nullptr}) -> bool;
    auto calcEfficiency() -> double;
    auto calcOptimalCount() -> double;

public:
    SimpleMachineNode();
    SimpleMachineNode(size_t id);
    SimpleMachineNode(size_t id, double time, double fuel, std::vector<Ingredient> ins, std::vector<Ingredient> outs);
    virtual ~SimpleMachineNode() = default;
    auto draw() -> void override;
    virtual auto drawInspector() -> bool;
    virtual auto update() -> void;
    virtual auto syncPins() -> void;
    inline const std::vector<Ingredient> &getInList() const { return isReverseFlow ? outs : ins; }
    inline const std::vector<Ingredient> &getOutList() const { return isReverseFlow ? ins : outs; }
    inline std::vector<Ingredient> &getInList() { return isReverseFlow ? outs : ins; }
    inline std::vector<Ingredient> &getOutList() { return isReverseFlow ? ins : outs; }
    inline std::vector<Ingredient> &getOutListRef() { return outs; }
    inline ImFlow::Pin* getInListElement(size_t idx) const {return inPins[idx];}
    inline ImFlow::Pin* getOutListElement(size_t idx) const {return outPins[idx];}
    inline void setFlow(bool reversed) {isReverseFlow = reversed;}
    inline const size_t getId() const { return id; }
    inline void reverseFlow() {
        isReverseFlow = !isReverseFlow;
        setTitle(getTitle());
        setStyle(getColor());
    }
    const auto print() const -> void;
    virtual const auto getNodeType() const -> NodeType {return type;};
    virtual auto serialize() -> nlohmann::json;
    virtual auto deserialize(nlohmann::json data) -> void;
    const auto getInPinIndex(ImFlow::Pin *pin) const -> size_t;
    const auto getOutPinIndex(ImFlow::Pin *pin) const -> size_t;    
    const auto getTitle() -> std::string;
    virtual const auto getColor() -> std::shared_ptr<ImFlow::NodeStyle>;

};