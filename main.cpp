// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
#include "config.h"

extern bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
extern void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture);
extern  cv::Mat GetOcvImgFromOglImg(GLuint ogl_texture_id);
extern void glfw_error_callback(int error, const char* description);
extern void setup_docking(ImGuiViewport* viewpor);

int main(int, char**)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1060, 604, "Dear ImGui - OpenCV example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(2); // Enable vsync: (1)->60Hz, (2)-> 30Hz, (3)-> 20Hz  (with a 60Hz refresh monitor)

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
	ImFont* font = io.Fonts->AddFontFromFileTTF("../../imgui/fonts/Roboto-Medium.ttf", 18.0f);
    IM_ASSERT(font != NULL);

	//OpenGl textures and OpenCV images
	int image_width = 0, image_height = 0;
	GLuint image_texture = 0, ocv_image_texture = 0;
	cv::Mat in_image, out_image;
	bool ret = false;
	
    // Our state
	bool show_dialog = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Image Files (bmp,jpeg,tif,png,gif)\0*.bmp;*.jpeg;*.tif;*.tiff;*.png;*.gif\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

		//Set up Docking Area
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		setup_docking(viewport);

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", NULL, &show_dialog, !show_dialog))
				{
					// Display the Open dialog box. 
					if (GetOpenFileName(&ofn) == TRUE)
					{
						ret = LoadTextureFromFile(ofn.lpstrFile, &image_texture, &image_width, &image_height);
						IM_ASSERT(ret);
						in_image = GetOcvImgFromOglImg(image_texture);
					}
					else
						show_dialog = false;
				}
				if (ImGui::MenuItem("Close", NULL, false, show_dialog))
				{
					show_dialog = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// DockSpace
		ImGuiID mainDockspaceId = ImGui::GetID("MainDockspaceArea");
		ImGui::DockSpace(mainDockspaceId, ImVec2(0.0f, 0.0f), dockspace_flags);

		static auto firstTime = true;
		if (firstTime)
		{
			firstTime = false;
			ImGui::DockBuilderRemoveNode(mainDockspaceId);
			ImGui::DockBuilderAddNode(mainDockspaceId, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(mainDockspaceId, viewport->GetWorkSize());

			auto dockIdLeft = ImGui::DockBuilderSplitNode(mainDockspaceId, ImGuiDir_Left, 0.5f, nullptr, &mainDockspaceId);
			
			ImGui::DockBuilderDockWindow("Original Image", dockIdLeft);
			ImGui::DockBuilderDockWindow("OpenCV Image", mainDockspaceId);
			ImGui::DockBuilderFinish(mainDockspaceId);
		}
		ImGui::End();

        // Show two simple windows that we create ourselves. We use a Begin/End pair to created named windows.
        if(show_dialog & ret)
		{
			ImGui::Begin("Original Image");

			ImGui::Image((void*)(intptr_t)image_texture, ImVec2((float)image_width, (float)image_height));

			static int e = 0;
			static int ksize = 3;
			static int sharp = 3;
			static int threshold[2] = {10,100};
			ImGui::Text("Effect:"); ImGui::SameLine();
			ImGui::RadioButton("None", &e, 0); ImGui::SameLine();
			ImGui::RadioButton("Gray Scale", &e, 1); ImGui::SameLine();
			ImGui::RadioButton("Blur", &e, 2); ImGui::SameLine();
			ImGui::RadioButton("Sharp", &e, 3); ImGui::SameLine();
			ImGui::RadioButton("Edge Detection", &e, 4);

            ImGui::End();

			ImGui::Begin("OpenCV Image");
			switch (e)
			{
			case 0:
				out_image = in_image.clone();
				break;
			case 1:
				cv::cvtColor(in_image, out_image, cv::COLOR_BGR2GRAY);
				break;
			case 2:
				cv::blur(in_image, out_image, cv::Size(ksize, ksize));
				break;
			case 3:
				cv::GaussianBlur(in_image, out_image, cv::Size(0, 0), sharp);
				cv::addWeighted(in_image, 1.5, out_image, -0.5, 0, out_image);
				break;
			case 4:
				cv::Canny(in_image, out_image, threshold[0], threshold[1]);
				break;
			}
			BindCVMat2GLTexture(out_image, ocv_image_texture);
			ImGui::Image((void*)(intptr_t)ocv_image_texture, ImVec2((float)image_width, (float)image_height));
			if (e == 2)
			{
				ImGui::SetNextItemWidth(75);
				ImGui::SliderInt("blurring kernel size    ", &ksize, 1, 20);
				ImGui::SameLine();
			}
			if (e == 3)
			{
				ImGui::SetNextItemWidth(75);
				ImGui::SliderInt("sharpness    ", &sharp, 1, 10);
				ImGui::SameLine();
			}
			if (e == 4)
			{
				ImGui::SetNextItemWidth(125);
				ImGui::SliderInt2("thresholds    ", threshold, 10, 255);
				ImGui::SameLine();
			}
			ImGui::Text("Running at %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
