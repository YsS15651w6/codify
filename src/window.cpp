#define GLFW_INCLUDE_NONE
#include "../includes/backend.h"
#include "../lib/glfw/include/glfw/glfw3.h"
#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <string>
#include <filesystem>
#include <GL/gl.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

using namespace std; // not clean. but it works anyways!

#ifdef _WIN32
std::string getExecutableDir() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::filesystem::path(path).parent_path().string();
}
#else
std::string getExecutableDir() {
    char path[1024];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (count == -1) return ""; 
    path[count] = '\0';  // Null-terminate the string
    return std::filesystem::path(std::string(path)).parent_path().string();
}
#endif

extern "C" int run_gui(int argc, char **argv)
{
#ifdef _WIN32
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow != NULL) CloseWindow(consoleWindow);
#endif
    
    if (!glfwInit()) {
#ifdef _WIN32
        MessageBoxW(NULL, L"Failed to initialize GLFW", L"Error", MB_OK | MB_ICONERROR);
#else
        fprintf(stderr, "Failed to initialize GLFW\n");
#endif
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Codify", NULL, NULL);
    if (!window) {
#ifdef _WIN32
        MessageBoxW(NULL, L"Failed to create GLFW window", L"Error", MB_OK | MB_ICONERROR);
#else
        fprintf(stderr, "Failed to create GLFW window\n");
#endif
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // FIXED: Move text buffers outside the loop and make them persistent for each format
    // Create arrays to store input/output for each format
    char (*input_texts)[4096] = new char[format_map_count][4096];
    char (*output_texts)[4096] = new char[format_map_count][4096];
    
    ImGuiIO& io = ImGui::GetIO();
    std::string execDir = getExecutableDir();
    std::string fontPath = execDir + "/assets/dejavu.ttf";
    ImFont* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, NULL, io.Fonts->GetGlyphRangesDefault());
    // Initialize all buffers to empty strings
    for (size_t i = 0; i < format_map_count; ++i) {
        input_texts[i][0] = '\0';
        output_texts[i][0] = '\0';
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImVec2 displaySize = io.DisplaySize;
        if (!font) {
            // we done fucked up... this should never happen but just in case, let's handle it gracefully
            fprintf(stderr, "Failed to load font!\n");
        }
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(displaySize);
        ImGui::Begin("CodifyMain", nullptr, 
            ImGuiWindowFlags_NoDecoration | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoSavedSettings | 
            ImGuiWindowFlags_NoBringToFrontOnFocus);
        
        ImGui::SetWindowFontScale(1.5f);
        ImVec2 titleSize = ImGui::CalcTextSize("Codify");
        ImGui::SetCursorPosX((displaySize.x - titleSize.x * 1.5f) * 0.5f);
        ImGui::TextUnformatted("Codify");
        ImGui::SetWindowFontScale(1.25f);
        std::string verStr = "Version v" + std::string(version);
        ImVec2 vSize = ImGui::CalcTextSize(verStr.c_str());
        ImGui::SetCursorPosX((displaySize.x - vSize.x * 1.25f) * 0.5f);
        ImGui::TextUnformatted(verStr.c_str());
        ImGui::Separator();
        ImGui::SetWindowFontScale(1.0f);
        // Grid-like layout with better responsive design
        const float minCellWidth = 400.0f; // Increased minimum width for buttons
        const float padding = 20.0f;
        float availableWidth = displaySize.x - padding;
        int columns = max(1, (int)(availableWidth / minCellWidth));
        
        // Calculate actual items that will be displayed to avoid empty columns
        int actualColumns = min(columns, (int)format_map_count);
        
        if (ImGui::BeginTable("FormatGrid", actualColumns, ImGuiTableFlags_SizingStretchSame)) {
            size_t itemsPerColumn = (format_map_count + actualColumns - 1) / actualColumns;
            size_t index = 0;
            
            for (size_t row = 0; row < itemsPerColumn && index < format_map_count; ++row) {
                ImGui::TableNextRow();
                
                for (int col = 0; col < actualColumns && index < format_map_count; ++col) {
                    ImGui::TableSetColumnIndex(col);
                    
                    const StringMap &fmt = format_map[index];
                    
                    // Use persistent buffers for this specific format
                    char *input_text = input_texts[index];
                    char *output_text = output_texts[index];

                    ImGui::Text("Format: %s", fmt.value);
                    ImGui::InputTextMultiline(
                        ("Input##" + std::to_string(index)).c_str(), 
                        input_text, 
                        4096,
                        ImVec2(-1.0f, 100.0f), 
                        ImGuiInputTextFlags_AllowTabInput);
                    
                    // Button layout - stack vertically for better fit
                    float availableButtonWidth = ImGui::GetContentRegionAvail().x;
                    float buttonWidth = (availableButtonWidth - 10.0f) / 2.0f; // Two buttons per row
                    
                    std::string encodeLabel = "Encode to " + std::string(fmt.value);
                    if (ImGui::Button((encodeLabel + "##" + std::to_string(index)).c_str(), ImVec2(buttonWidth, 0))) {
                        char *encoded = (char *)encode(fmt.key, input_text);
                        if (encoded) {
                            strncpy(output_text, encoded, 4095);
                            output_text[4095] = '\0';
                            free(encoded);
                        } else {
                            // Handle encoding failure
                            strncpy(output_text, "Encoding failed", 4095);
                            output_text[4095] = '\0';
                        }
                    }
                    
                    ImGui::SameLine();
                    if (ImGui::Button(("Decode##" + std::to_string(index)).c_str(), ImVec2(buttonWidth, 0))) {
                        char *decoded = (char*)decode(fmt.key, input_text);
                        if (decoded) {
                            strncpy(output_text, decoded, 4095);
                            output_text[4095] = '\0';
                            free(decoded);
                        } else {
                            // Handle decoding failure
                            strncpy(output_text, "Decoding failed", 4095);
                            output_text[4095] = '\0';
                        }
                    }
                    
                    // Second row of buttons
                    if (ImGui::Button(("Clear All##" + std::to_string(index)).c_str(), ImVec2(buttonWidth, 0))) {
                        input_text[0] = '\0';
                        output_text[0] = '\0';
                    }
                    
                    ImGui::SameLine();
                    if (ImGui::Button(("Copy Output##" + std::to_string(index)).c_str(), ImVec2(buttonWidth, 0))) {
                        ImGui::SetClipboardText(output_text);
                    }
                    
                    ImGui::InputTextMultiline(
                        ("Output##" + std::to_string(index)).c_str(), 
                        output_text, 
                        4096, 
                        ImVec2(-1.0f, 100.0f), 
                        ImGuiInputTextFlags_ReadOnly);
                    
                    ++index;
                }
            }
            ImGui::EndTable();
        }
        ImGui::Separator();
        ImGui::SetWindowFontScale(1.25f);
        ImGui::TextUnformatted("To run as CLI, run from terminal with argument 1 cli; for help, run with -h flag.");
        ImGui::End();
        
        // Render the frame
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    
    // Cleanup
    delete[] input_texts;
    delete[] output_texts;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}