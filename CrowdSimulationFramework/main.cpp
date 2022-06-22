#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))


#include"imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imguifilesystem.h"

#include "gl3w.h"
#include<GLFW\glfw3.h>

#include<iostream>
#include<string>
#include<vector>
#include<sstream>

#include <windows.h>

#include "GlWindow.h"


const char* glsl_version = "#version 130";

GlWindow* win;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;
double m_lastMouseX;
double m_lastMouseY;
double cx, cy;
int frameMin;
extern int frameMax;
bool setFirst = true;
extern int order = 0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


static void ShowExampleAppMainMenuBar(bool* p_open);
static void ShowExampleMenuFile();
void agentPosAdd(double tempX, double tempY);


static bool show_app_layout = false;

int m_amount = 0;						//객체의 수
int randAgentNum;
int randRange;

int width = 800;
int height = 800;

bool firstMouse = true;
bool firstPosMouse = true;
bool isClicked = false;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float lastPosX = 800.0f / 2.0;
float lastPosY = 600.0 / 2.0;
float zoom = 45.0f;

float m_zoom = 0.1f;

std::vector<glm::vec2> selectPos;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	//win->setSize(width, height);
	//win->setAspect(width / (float)height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active = ImGui::IsItemActive();   // Held

	ImGui::GetIO().MousePos.x = float(xposIn);
	ImGui::GetIO().MousePos.y = float(yposIn);

	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);


	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		//cam.ProcessMouseMovement(xoffset, yoffset);
	}
	else {
		firstMouse = true;
	}

	//마우스 휠 버튼 클릭 감지
	int button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
	if (button == GLFW_PRESS) {
		//캠 위치 이동
		if (firstPosMouse)
		{
			lastPosX = xpos;
			lastPosY = ypos;
			firstPosMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastPosX = xpos;
		lastPosY = ypos;

		//cam.cameraPositionMove(xoffset, yoffset, deltaTime);
	}
	else {
		firstPosMouse = true;
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="p_open"></param>
static void ShowExampleAppMainMenuBar(bool* p_open)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

static void ShowExampleMenuFile()
{
	if (ImGui::MenuItem("Basic"))
	{
		//win->initialize();
	}
	if (ImGui::MenuItem("Line"))
	{
		//win->initialize(1);
		//		win->changeModel("nanosuit/nanosuit.obj");
	}
	if (ImGui::MenuItem("close"))
	{
		//win->changeModel("");
	}
	if (ImGui::BeginMenu("Open Recent"))
	{
		ImGui::MenuItem("fish_hat.c");
		ImGui::MenuItem("fish_hat.inl");
		ImGui::MenuItem("fish_hat.h");
		if (ImGui::BeginMenu("More.."))
		{
			ImGui::MenuItem("Hello");
			ImGui::MenuItem("Sailor");
			if (ImGui::BeginMenu("Recurse.."))
			{
				ShowExampleMenuFile();
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

}

//마우스위치를 OpenGl 위치로 변환
void transToModelPosition(double x, double y)
{
	double halfW = width / 2;
	double halfH = height / 2;
	double tempX, tempY;

	//1사분면에 있을 경우
	if (x > halfW && y < halfH)
	{
		tempX = x - halfW;
		tempY = halfH - y;
	}
	//2사분면에 있을 경우
	else if (x < halfW && y < halfH)
	{
		tempX = -1 * (halfW - x);
		tempY = halfH - y;
	}
	//3사분면에 있을 경우
	else if (x<halfW && y>halfH)
	{
		tempX = -1 * (halfW - x);
		tempY = -1 * (y - halfH);
	}
	//4사분면에 있을 경우
	else if (x > halfW && y > halfH)
	{
		tempX = x - halfW;
		tempY = -1 * (y - halfH);
	}
	else if (x == halfW)
	{
		tempX = 0;
		if (y < halfH)
			tempY = halfH - y;
		else if (y > halfH)
			tempY = -1 * (y - halfH);
		else
			tempY = 0;
	}
	else if (y == halfH)
	{
		tempY = 0;
		if (x < halfW)
			tempX = -1 * (halfW - x);
		else if (x > halfW)
			tempX = x - halfW;
		else
			tempX = 0;
	}

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//버튼 눌릴때
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;
		if (ImGui::GetIO().MousePos.x <= width && ImGui::GetIO().MousePos.y <= height)
		{
			transToModelPosition(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
		}
	}

	//마우스 왼쪽 버튼 클릭
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}
	//마우스 오른쪽 버튼 클릭
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}
	//마우스 휠 클릭
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mbutton_down = true;
		else if (GLFW_RELEASE == action)
			mbutton_down = false;
	}
}

void mouseDragging(double width, double height)		//카메라 회전
{

	if (ImGui::GetIO().WantCaptureKeyboard == false) {
		if (lbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			//win->m_viewer->rotate(fractionChangeX, fractionChangeY);
		}
		else if (mbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			//win->m_viewer->zoom(fractionChangeY);
		}
		else if (rbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			//win->m_viewer->translate(-fractionChangeX, -fractionChangeY, 1);
		}
	}

	m_lastMouseX = cx;
	m_lastMouseY = cy;

}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);		//OpenGl 4.0을 쓰겠다
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);		//3.0
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//CORE_PROFILE : 최신의 mordern한 기능만 가져다가 쓰는것		COMFORT_PROFILE : 예전 버전도 있음

	window = glfwCreateWindow(width + 200, height, "OpenGL FrameWork", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	if (gl3wInit()) {
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}

	if (!gl3wIsSupported(4, 3)) {
		fprintf(stderr, "OpenGL 4.0 not supported\n");
		return -1;
	}

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),glGetString(GL_SHADING_LANGUAGE_VERSION));

	

	/*static bool no_titlebar = false;
	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
*/

	win = new GlWindow(width, height);


	bool show_test_window = true;
	bool show_another_window = true;
	bool show_menubar = false;
	bool show_app_main_menu_bar = true;

	ImVec4 clear_color = ImColor(144, 144, 144);

	double previousTime = glfwGetTime();
	int frameCount = 0;
	double lastTime;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	io.Fonts->AddFontDefault();

	// Setup ImGui binding
// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetWindowTitle(window, "CrowdSimulationFramework");
	glfwSetWindowSizeCallback(window, window_size_callback);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static float f = 0.5f;

		static glm::vec4 RGB = glm::vec4(1, 1, 1, 1);
		static glm::vec3 LightPos = glm::vec3(50, 50, 50);

		static bool isAlpha = false;

		static bool show_app_main_menu_bar = true;

		if (show_app_main_menu_bar) ShowExampleAppMainMenuBar(&show_app_main_menu_bar);

		static bool no_titlebar = false;

		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;

		//win->draw(f, RGB, LightPos, m_amount, m_zoom);

		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		//glfwWaitEvents();
		mouseDragging(display_w, display_h);

	}

	//Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
