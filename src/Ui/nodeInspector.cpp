#include "nodeInspector.hpp"

auto NodeInspector::setNode(std::shared_ptr<SimpleMachineNode> node) -> void
{
    if (!node)
    {
        selectedNode.reset();
        return;
    }

    selectedNode = node;
    isShowing = true;
}

const auto NodeInspector::draw(ImFlow::ImNodeFlow &grid, bool canDraw) -> void const
{
    if(!canDraw)
        return;
    if (isHiddenByKeys)
        return;
    if (!isShowing)
        return;

    float screenW = ImGui::GetIO().DisplaySize.x;
    float screenH = ImGui::GetIO().DisplaySize.y;

    ImGui::SetNextWindowPos(ImVec2(screenW - inspectorWidth, menuHeight));
    ImGui::SetNextWindowSize(ImVec2(inspectorWidth, screenH - menuHeight));

    const auto flags = ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoResize |
                       ImGuiWindowFlags_NoCollapse;

    if (ImGui::Begin("Node Inspector", &isShowing, flags))
    {
        if (auto node = selectedNode.lock())
        {
            ImGui::SetWindowFontScale(1.5f);
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "UID: 0x%lX", node->getUID());
            ImGui::Separator();
            ImGui::Spacing();

            auto changed = node->drawInspector();

            if (changed)
            {
                auto edges = getOldEdges(node, grid);
                node->syncPins();
                for(auto [left, right, leftNode, rightNode, weight] : edges) {
                    auto leftPin = leftNode->getOutListElement(left);
                    auto rightPin = rightNode->getInListElement(right);
                    if(leftPin && rightPin) {
                        leftPin->createLink(rightPin);
                    }
                }
            }
        }
        else
        {
            isShowing = false;
            isHiddenByKeys = false;
        }
    }
    ImGui::End();
}

auto NodeInspector::update() -> void
{
    if (!isShowing)
    {
        selectedNode.reset();
    }
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_I))
    {
        isHiddenByKeys = !isHiddenByKeys;
    }
}

auto NodeInspector::setShow(bool isShowings) -> void
{
    this->isShowing = isShowing;
}

const auto NodeInspector::getShow() -> bool const
{
    return this->isShowing;
}

auto NodeInspector::setHiddenByKeys(bool isHidden) -> void
{
    this->isHiddenByKeys = isHidden;
}

const auto NodeInspector::getHiddenByKeys() -> bool const
{
    return this->isHiddenByKeys;
}

auto NodeInspector::getOldEdges(std::shared_ptr<SimpleMachineNode> node, ImFlow::ImNodeFlow &grid) -> std::vector<SavedLink>
{
    std::vector<SavedLink> edges{};
    for (auto link : grid.getLinks())
    {
        if (link.expired())
            continue;
        auto locked = link.lock();
        auto left = locked->left();
        auto right = locked->right();
        if (left->getParent()->getUID() == node->getUID() || right->getParent()->getUID() == node->getUID())
        {
            SavedLink savedLink{
                left->getUid(),
                right->getUid(),
                dynamic_cast<SimpleMachineNode*>(left->getParent()),
                dynamic_cast<SimpleMachineNode*>(right->getParent()),
            };
            edges.push_back({savedLink});
        }
    }
    return edges;
}
