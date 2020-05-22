// Example program using Dear ImGui (OpenGL3/GLFW) and OpenCV

#include "App.hpp"

int main(int, char **)
{
	App app;
	app.Init();
	//app.show_demo_window_ = true;
	app.Run();
	return 0;
}