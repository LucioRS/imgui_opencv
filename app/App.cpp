#include "App.hpp"

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

App::App()
{
	original_texture_id_ = -1;
	modified_texture_id_ = -1;
	show_images_ = false;
	firstTime_ = true;
	show_demo_window_ = false;
	clear_color_ = ImVec4(0.05f, 0.15f, 0.20f, 1.00f);
	optimal_image_width_height_ = 512;
	windows_width_ = optimal_image_width_height_ + 18;
	windows_height_ = optimal_image_width_height_ + 68;
	radio_ = 0;
	radio_ant_ = 0;
	ksize_ = 3;
	sharpness_ = 3;
	threshold_[0] = 10;
	threshold_[1] = 100;
	imageWindows_flags_ = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
}

App::~App()
{
}

int App::Init()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if __APPLE__
	// GL 3.2 + GLSL 150
	const char *glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); //User can't resize window
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	monitor_mode_ = glfwGetVideoMode(monitor);
	// Create window with graphics context (In this case, (512+18) x 2 = 1060 width, 25(MenuBar) + (512+68) = 605 height, for 512 square image)
	window_ = glfwCreateWindow(windows_width_ * 2, 25 + windows_height_, "Dear ImGui - OpenCV example", NULL, NULL);
	max_window_width_ = (monitor_mode_->width - 100) / 2;  //100 margin
	max_image_width_ = max_window_width_ - 18;
	if (window_ == NULL)
		return 1;
	glfwMakeContextCurrent(window_);
	glfwSwapInterval(1); // Enable vsync: (1)->60Hz, (2)-> 30Hz, (3)-> 20Hz  (with a 60Hz refresh monitor)

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
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window_, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	ImFont* font = io.Fonts->AddFontFromFileTTF("../../fonts/Roboto-Medium.ttf", 18.0f);
	IM_ASSERT(font != NULL);

	// Open File Dialog setup(only for Windows!!) - comment out if not in Windows ///
	// Initialize OPENFILENAME
	ZeroMemory(&ofn_, sizeof(ofn_));
	ofn_.lStructSize = sizeof(ofn_);
	ofn_.hwndOwner = NULL;
	ofn_.lpstrFile = szFile_;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn_.lpstrFile[0] = '\0';
	ofn_.nMaxFile = sizeof(szFile_);
	ofn_.lpstrFilter = "Image Files (bmp,dib,jpeg,jpg,jpe,jp2,png,webp,pbm,pgm,ppm,pxm,pnm,pfm,sr,tif,tiff,exr,hdr)"
		"\0*.bmp;*.dib;*.jpeg;*.jpg;*.jpe;*.jp2;*.png;*.webp;*.pbm;*.pgm;*.ppm;*.pxm;*.pnm;*.pfm;*.sr;*.tif;*.tiff;*.exr;*.hdr\0";
	ofn_.nFilterIndex = 1;
	ofn_.lpstrFileTitle = NULL;
	ofn_.nMaxFileTitle = 0;
	ofn_.lpstrInitialDir = "../../images/";
	ofn_.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return 0;
}

void App::UpdateTexture(bool first)
{
	unsigned char *data;
	if (first)
	{
		data = resized_image_.ptr();
		if (original_texture_id_ == -1)
			glGenTextures(1, &original_texture_id_);
		glBindTexture(GL_TEXTURE_2D, original_texture_id_);
		glPixelStorei(GL_UNPACK_ALIGNMENT, (resized_image_.step & 3) ? 1 : 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width_, image_height_, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

		if (modified_texture_id_ == -1)
			glGenTextures(1, &modified_texture_id_);
		glBindTexture(GL_TEXTURE_2D, modified_texture_id_);
		glPixelStorei(GL_UNPACK_ALIGNMENT, (resized_image_.step & 3) ? 1 : 4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width_, image_height_, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		data = modified_image_.ptr();
		glBindTexture(GL_TEXTURE_2D, modified_texture_id_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width_, image_height_, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
	}
}

void App::ShowMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", NULL, &show_images_, !show_images_))
			{
				if (GetOpenFileName(&ofn_) == TRUE) // Display the Open dialog box (only for Windows!!) ///
				{
					original_image_ = cv::imread(ofn_.lpstrFile);
					//original_image_ = cv::imread("../../images/Lenna.png"); //Open File directly without using dialog
					if (!original_image_.empty())
					{
						float resized_height;
						float ratio = static_cast<float>(original_image_.size().width) / original_image_.size().height;
						if(ratio * optimal_image_width_height_ > max_image_width_)
							resized_height = static_cast<float>(max_image_width_) / original_image_.size().width * original_image_.size().height;
						else
							resized_height = static_cast<float>(optimal_image_width_height_);
						//resized_height = 470;
						double scale = resized_height / original_image_.size().height;
						cv::resize(original_image_, resized_image_, cv::Size(0, 0), scale, scale);
						image_width_ = resized_image_.size().width;
						image_height_ = resized_image_.size().height;
						UpdateTexture(true);

						windows_height_ = resized_image_.size().height + 68;
						if (resized_image_.size().width > optimal_image_width_height_)
							windows_width_ = resized_image_.size().width + 18;
						else
							windows_width_ = optimal_image_width_height_ + 18;

						glfwSetWindowSize(window_, windows_width_* 2, 25 + windows_height_);
					}
				}
				else
					show_images_ = false;
			}
			if (ImGui::MenuItem("Close", NULL, false, show_images_))
			{
				radio_ = 0;
				show_images_ = false;
				glfwSetWindowSize(window_, (optimal_image_width_height_ + 18) * 2, 25 + 68 + optimal_image_width_height_);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void App::ShowImages()
{
	if (show_demo_window_)
		ImGui::ShowDemoWindow(&show_demo_window_);

	if (show_images_)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 25));
		ImGui::SetNextWindowSize(ImVec2((float)windows_width_, (float)windows_height_));
		ImGui::Begin("Original Image", NULL, imageWindows_flags_);
		ImGui::Image((void*)(intptr_t)original_texture_id_, ImVec2((float)image_width_, (float)image_height_));

		ImGui::Text("Effect:"); ImGui::SameLine();
		ImGui::RadioButton("None", &radio_, 0); ImGui::SameLine();
		ImGui::RadioButton("Gray Scale", &radio_, 1); ImGui::SameLine();
		ImGui::RadioButton("Blur", &radio_, 2); ImGui::SameLine();
		ImGui::RadioButton("Sharp", &radio_, 3); ImGui::SameLine();
		ImGui::RadioButton("Edge Detection", &radio_, 4);

		radio_changed_ = false;
		if (radio_ != radio_ant_)
		{
			radio_changed_ = true;
			radio_ant_ = radio_;
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2((float)windows_width_, 25));
		ImGui::SetNextWindowSize(ImVec2((float)windows_width_, (float)windows_height_));
		ImGui::Begin("Modified Image", NULL, imageWindows_flags_);
		ImGui::Image((void*)(intptr_t)modified_texture_id_, ImVec2((float)image_width_, (float)image_height_));

		switch (radio_)
		{
		case 0:
			if (radio_changed_)
			{
				modified_image_ = resized_image_.clone();
				UpdateTexture();
			}
			break;
		case 1:
			if (radio_changed_)
			{
				cv::cvtColor(resized_image_, modified_image_, cv::COLOR_RGB2GRAY);
				cv::cvtColor(modified_image_, modified_image_, cv::COLOR_GRAY2RGB);
				UpdateTexture();
			}
			break;
		case 2:
			ImGui::SetNextItemWidth(75);
			ImGui::SliderInt("blurring kernel size    ", &ksize_, 1, 20);
			ImGui::SameLine();
			if (radio_changed_ || ImGui::IsItemEdited())
			{
				cv::blur(resized_image_, modified_image_, cv::Size(ksize_, ksize_));
				UpdateTexture();
			}
			break;
		case 3:
			ImGui::SetNextItemWidth(100);
			ImGui::SliderInt("sharpness    ", &sharpness_, 1, 20);
			ImGui::SameLine();
			if (radio_changed_ || ImGui::IsItemEdited())
			{
				cv::GaussianBlur(resized_image_, modified_image_, cv::Size(0, 0), sharpness_);
				cv::addWeighted(resized_image_, 1.5, modified_image_, -0.5, 0, modified_image_);
				UpdateTexture();
			}
			break;
		case 4:
			ImGui::SetNextItemWidth(125);
			ImGui::SliderInt2("thresholds    ", threshold_, 10, 300);
			ImGui::SameLine();
			if (radio_changed_ || ImGui::IsItemEdited())
			{
				cv::Canny(resized_image_, modified_image_, threshold_[0], threshold_[1]);
				cv::cvtColor(modified_image_, modified_image_, cv::COLOR_GRAY2RGB);
				UpdateTexture();
			}
			break;
		}
		ImGui::Text("Running at %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

void App::Run()
{
	while (!glfwWindowShouldClose(window_))
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

		ShowMenu();
		ShowImages();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window_, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color_.x, clear_color_.y, clear_color_.z, clear_color_.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window_);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window_);
	glfwTerminate();
}
