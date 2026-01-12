cmake_minimum_required(VERSION 3.16)
project(MyApp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. GLFW ---
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw)
find_package(OpenGL REQUIRED)

# --- 2. ImGui ---
add_library(imgui STATIC
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_glfw.cpp
    third_party/imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC third_party/imgui third_party/imgui/backends)
target_link_libraries(imgui PUBLIC glfw OpenGL::GL)

# FIX FOR THE ERROR:
target_compile_definitions(imgui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

# --- 3. ImNodeFlow ---
add_library(imnodeflow STATIC third_party/ImNodeFlow/src/ImNodeFlow.cpp)
target_include_directories(imnodeflow PUBLIC third_party/ImNodeFlow/include)
target_link_libraries(imnodeflow PUBLIC imgui)

# FIX FOR THE ERROR:
target_compile_definitions(imnodeflow PUBLIC IMGUI_DEFINE_MATH_OPERATORS)

add_library(app_lib STATIC
    src/NodeController/nodeEditor.cpp
    src/Nodes/factoryNode.cpp
    src/Nodes/simpleNode.cpp
    src/Nodes/simpleMahcineNode.cpp
    src/Util/digraph.cpp
)

target_include_directories(app_lib PUBLIC src)
target_link_libraries(app_lib PUBLIC imgui imnodeflow)

# --- 4. Main Application ---
add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE imgui imnodeflow glfw OpenGL::GL app_lib)

if (WIN32)
    target_link_libraries(app PRIVATE gdi32 user32 shell32 imm32)
elseif (UNIX)
    target_link_libraries(app PRIVATE dl pthread)
endif()