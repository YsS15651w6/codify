cmake_minimum_required(VERSION 3.10)
project(Codify VERSION 1.1 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/includes
    ${PROJECT_SOURCE_DIR}/lib/glfw/include
    ${PROJECT_SOURCE_DIR}/lib/imgui
    ${PROJECT_SOURCE_DIR}/lib/imgui/backends
)

set(SOURCES
    src/main.c
    src/cli.c
    src/window.cpp
    includes/backend.c
    codecs/pigl/pigl.c
    codecs/ubdb/ubdb.c
    codecs/eclcrypt/eclcrypt.c
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(codify ${SOURCES})

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

if(WIN32)
    target_link_libraries(codify glfw opengl32 gdi32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(codify glfw ${OPENGL_LIBRARIES} X11 Xrandr Xi Xxf86vm Xcursor pthread)
endif()
