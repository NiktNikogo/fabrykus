#include <imgui.h>
#include <functional>
struct MenuBuilder {
  float fontSize;
    bool currentMenuOpen = false;

    MenuBuilder(float size);
    ~MenuBuilder();

    auto beginMenu(const char* label) -> MenuBuilder&;

    auto addItem(const char* label, const char* shortcut, std::function<void()> action) -> MenuBuilder&;

    auto endMenu() ->MenuBuilder&;
};