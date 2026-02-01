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

const auto NodeInspector::draw() -> void const
{
    if(isHiddenByKeys) 
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

            node->drawInspector();
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
