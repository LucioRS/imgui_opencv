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

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") //Comment out to enable CONSOLE WINDOW

class App
{
public:
	App();
	~App();

	int Init();
	void Render();
	void SetupDockArea();
	void ShowDockArea();
	void ShowMenu();
	void ShowImages();
	void UpdateTexture(bool first = false);
	bool show_demo_window_;

private:
	GLFWwindow* window_;
	ImGuiViewport* viewport_;
	ImGuiDockNodeFlags dockspace_flags_;
	ImGuiWindowFlags window_flags_;
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

