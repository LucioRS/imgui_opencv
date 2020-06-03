#pragma once

// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <windows.h>	//To use window's dialog to select image file, comment out to open file without dialog
#include <opencv2/opencv.hpp>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#define min(a,b) ((a)<(b)?(a):(b))	//Caution when using these macros, see:
#define max(a,b) ((a)>(b)?(a):(b))	//https://dustri.org/b/min-and-max-macro-considered-harmful.html

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") //Comment out to enable CONSOLE WINDOW

class App
{
public:
	App();
	~App();

	int Init();
	void Run();
	void ShowMenu();
	void ShowImages();
	void UpdateTexture(bool first = false);
	bool show_demo_window_;

private:
	GLFWwindow* window_;
	const GLFWvidmode* monitor_mode_;
	ImGuiWindowFlags imageWindows_flags_;
	int max_window_width_;
	int windows_width_, windows_height_;
	int optimal_image_width_height_;
	int max_image_width_;
	// comment out to open file without dialog (No windows.h)
	OPENFILENAME ofn_;	// common dialog box structure
	ImVec4 clear_color_;
	char szFile_[260];	// buffer for file name
	GLuint original_texture_id_;
	GLuint modified_texture_id_;
	int image_width_, image_height_;
	cv::Mat original_image_;
	cv::Mat resized_image_;
	cv::Mat modified_image_;
	bool show_images_;
	bool firstTime_;
	int radio_;
	int radio_ant_;
	bool radio_changed_;
	int ksize_;
	int sharpness_;
	int threshold_[2];
};

static ImVec4 base = ImVec4(0.502f, 0.075f, 0.256f, 1.0f);
static ImVec4 bg = ImVec4(0.200f, 0.220f, 0.270f, 1.0f);
static ImVec4 text = ImVec4(0.860f, 0.930f, 0.890f, 1.0f);
static float high_val = 0.8f;
static float mid_val = 0.5f;
static float low_val = 0.3f;
static float window_offset = -0.2f;

inline ImVec4 make_high(float alpha) {
    ImVec4 res(0, 0, 0, alpha);
    ImGui::ColorConvertRGBtoHSV(base.x, base.y, base.z, res.x, res.y, res.z);
    res.z = high_val;
    ImGui::ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
    return res;
}

inline ImVec4 make_mid(float alpha) {
    ImVec4 res(0, 0, 0, alpha);
    ImGui::ColorConvertRGBtoHSV(base.x, base.y, base.z, res.x, res.y, res.z);
    res.z = mid_val;
    ImGui::ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
    return res;
}

inline ImVec4 make_low(float alpha) {
    ImVec4 res(0, 0, 0, alpha);
    ImGui::ColorConvertRGBtoHSV(base.x, base.y, base.z, res.x, res.y, res.z);
    res.z = low_val;
    ImGui::ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
    return res;
}

inline ImVec4 make_bg(float alpha, float offset = 0.f) {
    ImVec4 res(0, 0, 0, alpha);
    ImGui::ColorConvertRGBtoHSV(bg.x, bg.y, bg.z, res.x, res.y, res.z);
    res.z += offset;
    ImGui::ColorConvertHSVtoRGB(res.x, res.y, res.z, res.x, res.y, res.z);
    return res;
}

inline ImVec4 make_text(float alpha) {
    return ImVec4(text.x, text.y, text.z, alpha);
}

inline void theme_generator() {
    ImGui::Begin("Theme generator");
    ImGui::ColorEdit3("base", (float*)&base, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("bg", (float*)&bg, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit3("text", (float*)&text, ImGuiColorEditFlags_PickerHueWheel);
    ImGui::SliderFloat("high", &high_val, 0, 1);
    ImGui::SliderFloat("mid", &mid_val, 0, 1);
    ImGui::SliderFloat("low", &low_val, 0, 1);
    ImGui::SliderFloat("window", &window_offset, -0.4f, 0.4f);

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text] = make_text(0.78f);
    style.Colors[ImGuiCol_TextDisabled] = make_text(0.28f);
    style.Colors[ImGuiCol_WindowBg] = make_bg(1.00f, window_offset);
    style.Colors[ImGuiCol_ChildBg] = make_bg(0.58f);
    style.Colors[ImGuiCol_PopupBg] = make_bg(0.9f);
    style.Colors[ImGuiCol_Border] = make_bg(0.6f, -0.05f);
    style.Colors[ImGuiCol_BorderShadow] = make_bg(0.0f, 0.0f);
    style.Colors[ImGuiCol_FrameBg] = make_bg(1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = make_mid(0.78f);
    style.Colors[ImGuiCol_FrameBgActive] = make_mid(1.00f);
    style.Colors[ImGuiCol_TitleBg] = make_low(1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = make_high(1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = make_bg(0.75f);
    style.Colors[ImGuiCol_MenuBarBg] = make_bg(0.47f);
    style.Colors[ImGuiCol_ScrollbarBg] = make_bg(1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = make_low(1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = make_mid(0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = make_mid(1.00f);
    style.Colors[ImGuiCol_CheckMark] = make_high(1.00f);
    style.Colors[ImGuiCol_SliderGrab] = make_bg(1.0f, .1f);
    style.Colors[ImGuiCol_SliderGrabActive] = make_high(1.0f);
    style.Colors[ImGuiCol_Button] = make_bg(1.0f, .2f);
    style.Colors[ImGuiCol_ButtonHovered] = make_mid(1.00f);
    style.Colors[ImGuiCol_ButtonActive] = make_high(1.00f);
    style.Colors[ImGuiCol_Header] = make_mid(0.76f);
    style.Colors[ImGuiCol_HeaderHovered] = make_mid(0.86f);
    style.Colors[ImGuiCol_HeaderActive] = make_high(1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered] = make_mid(0.78f);
    style.Colors[ImGuiCol_ResizeGripActive] = make_mid(1.00f);
    style.Colors[ImGuiCol_PlotLines] = make_text(0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered] = make_mid(1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = make_text(0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = make_mid(1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = make_mid(0.43f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = make_bg(0.73f);
    style.Colors[ImGuiCol_Tab] = make_bg(0.40f);
    style.Colors[ImGuiCol_TabHovered] = make_high(1.00f);
    style.Colors[ImGuiCol_TabActive] = make_mid(1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = make_bg(0.40f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = make_bg(0.70f);
    style.Colors[ImGuiCol_DockingPreview] = make_high(0.30f);

    if (ImGui::Button("Export")) {
        ImGui::LogToTTY();
        ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;\n");
        for (int i = 0; i < ImGuiCol_COUNT; i++) {
            const ImVec4& col = style.Colors[i];
            const char* name = ImGui::GetStyleColorName(i);
            ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n",
                name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
        }
        ImGui::LogFinish();
    }
    ImGui::End();
}