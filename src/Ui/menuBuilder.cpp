#include "menuBuilder.hpp"

MenuBuilder::MenuBuilder(float size) : fontSize(size)
{
    ImGui::BeginMainMenuBar();
    ImGui::SetWindowFontScale(fontSize);
}

MenuBuilder::~MenuBuilder()
{
    ImGui::EndMainMenuBar();
}

auto MenuBuilder::beginMenu(const char *label) -> MenuBuilder &
{
    currentMenuOpen = ImGui::BeginMenu(label);
    if (currentMenuOpen)
    {
        ImGui::SetWindowFontScale(fontSize);
    }
    return *this;
}

auto MenuBuilder::addItem(const char *label, const char *shortcut, std::function<void()> action) -> MenuBuilder &
{
    if (currentMenuOpen && ImGui::MenuItem(label, shortcut))
    {
        if (action)
            action();
    }
    return *this;
}

auto MenuBuilder::endMenu() -> MenuBuilder &
{
    if (currentMenuOpen)
    {
        ImGui::EndMenu();
        currentMenuOpen = false;
    }
    return *this;
}