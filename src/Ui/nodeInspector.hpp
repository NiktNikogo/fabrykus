#include <memory>
#include <imgui.h>
#include <ImNodeFlow.h>

#include "Nodes/simpleMachineNode.hpp"



class NodeInspector {
private:
    std::weak_ptr<SimpleMachineNode> selectedNode;
    bool isShowing;
    bool isHiddenByKeys;
    struct SavedLink {
        size_t leftIdx;
        size_t rightIdx;
        SimpleMachineNode* leftNode;
        SimpleMachineNode* rightNode;
    };
public:
    const float inspectorWidth = 500.0f;
    const float menuHeight = 25.0f;

public:
    NodeInspector() : 
        isShowing(false),
        selectedNode(),
        isHiddenByKeys(false)
    {}
    
    auto setNode(std::shared_ptr<SimpleMachineNode> node) -> void;
    const auto draw(ImFlow::ImNodeFlow& grid, bool canDraw) -> void const;
    auto update() -> void;
    auto setShow(bool isShowings) -> void;
    const auto getShow() -> bool const;
    auto setHiddenByKeys(bool isHidden) -> void;
    const auto getHiddenByKeys() -> bool const;
    auto getOldEdges(std::shared_ptr<SimpleMachineNode> node, ImFlow::ImNodeFlow& grid) -> std::vector<SavedLink>;
};