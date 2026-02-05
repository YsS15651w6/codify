cmake_minimum_required(VERSION 3.10)
project(Codify VERSION 1.2 LANGUAGES C CXX)

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
    codecs/eclcrypt_pwd/eclcrypt_pwd.c
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(codify ${SOURCES})
install(TARGETS codify
    RUNTIME DESTINATION bin
)
install(DIRECTORY src/assets/
    DESTINATION bin/assets
)


add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

if(WIN32)
    target_link_libraries(codify glfw opengl32 gdi32)
else()
    # Linux might cause some people to break their keyboards out of rage... good thing it's FOSS!
    find_package(OpenGL REQUIRED)
    target_link_libraries(codify glfw ${OPENGL_LIBRARIES} X11 Xrandr Xi Xxf86vm Xcursor pthread)
endif()

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "Codify")
set(CPACK_PACKAGE_VENDOR "RainDrippy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Codify - CLI + GUI tool")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "raindrippy.contact@protonmail.com")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
else()
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)