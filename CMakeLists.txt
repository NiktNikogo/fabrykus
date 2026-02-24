cmake_minimum_required(VERSION 3.16)
project(MyApp VERSION 0.1.0 LANGUAGES C CXX) 

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

find_package(Boost REQUIRED COMPONENTS graph) 
find_package(glfw3 CONFIG REQUIRED) 
find_package(imgui CONFIG REQUIRED) 
find_package(nlohmann_json CONFIG REQUIRED) 
find_package(OpenGL REQUIRED) 

# ImNodeFlow
add_library(imnodeflow STATIC third_party/ImNodeFlow/src/ImNodeFlow.cpp) 
target_include_directories(imnodeflow PUBLIC third_party/ImNodeFlow/include) 

target_link_libraries(imnodeflow PUBLIC imgui::imgui) 
target_compile_definitions(imnodeflow PUBLIC IMGUI_DEFINE_MATH_OPERATORS) 

# ImGuiFileDialog
add_library(imguifiledialog STATIC third_party/ImGuiFileDialog/ImGuiFileDialog.cpp) 
target_compile_definitions(imguifiledialog PRIVATE USE_STD_FILESYSTEM) 
target_include_directories(imguifiledialog PUBLIC third_party/ImGuiFileDialog) 
target_link_libraries(imguifiledialog PUBLIC imgui::imgui) 

# my staff
add_library(app_lib STATIC
    src/Internal/nodeEditorIO.cpp
    src/Ui/nodeEditor.cpp
    src/Ui/nodeInspector.cpp
    src/Ui/menuBuilder.cpp
    src/Nodes/simpleMachineNode.cpp
    src/Nodes/productNode.cpp
    src/Nodes/ingredientNode.cpp
    src/Util/digraph.cpp
    src/Util/nodeFactory.hpp
) 

target_include_directories(app_lib PUBLIC src) 
# linking
target_link_libraries(app_lib PUBLIC 
    imgui::imgui 
    imnodeflow 
    imguifiledialog 
    nlohmann_json::nlohmann_json
    Boost::graph
) 

# main
add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE 
    app_lib 
    glfw 
    imgui::imgui
    OpenGL::GL
)

if (WIN32)
    target_link_libraries(app PRIVATE gdi32 user32 shell32 imm32)
elseif (UNIX)
    target_link_libraries(app PRIVATE dl pthread)
endif()