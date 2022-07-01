#include "gl3w.h"
#include<GLFW\glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/dual_quaternion.hpp>

#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include <windows.h>

#include"imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imguifilesystem.h"
#include"imgui_user_rendering_interface.h"

#include "GlWindow.h"
#include"Camera.h"
#include <algorithm>

const char* glsl_version = "#version 130";

static int windowWidth = 1500;
static int windowHeight = 800;
static int screenWidth = 800;
static int screenHeight = 800;
static int inspectorWidth = 500;
static int hierachyWidth = 200;

GlWindow* win;
camera cam(glm::vec3(0.0f, -150.0f, 150.0f));

static bool show_app_layout = false;

int m_amount = 0;						//객체의 수
int randAgentNum;
int randRange;

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

double _frame_start_time;
double _delta_time;

template <class T>
class PingPongId
{
public:
	void swap() { _ping = pong(); }
	T ping() { return _ping; };
	T pong() { return (_ping + 1) % 2; }

private:
	T _ping;
};

int _frame_count{};
double _time_acc{};
double _cpu_time{};
double _cpu_time_min{ DBL_MAX };
double _cpu_time_max{};
double _cpu_time_avg{};
double _cpu_time_acc{};

GLuint64 _gpu_time{};
GLuint64 _gpu_time_min{ UINT_MAX };
GLuint64 _gpu_time_max{};
GLuint64 _gpu_time_avg{};
GLuint64 _gpu_time_acc{};

GLuint _gpu_timer_query[2]{};
PingPongId <size_t> _swap_id;

static void ShowExampleAppMainMenuBar(bool* p_open);
static void ShowExampleMenuFile();
static void ShowInspectorOverlay(bool* p_open);
static void ShowHierachyOverlay(bool* p_open);

std::vector<glm::vec2> selectPos;

//user rendering interface
imguiURI winURI;

using namespace std;

void drawStatsImGui(GlWindow* win, const double& time, const double& delta_time)
{
	constexpr size_t res = 32;

	const double refresh_interval_min = 0., refresh_interval_max = 5;

	static bool draw_time_plot = true;
	static double refresh_interval = 4.0;
	static bool show_gpu_ms = true;

	static bool freeze = false;

	static std::string cpu_histogram_text;
	static std::string gpu_histogram_text;
	static float cpu_time_data[res] = { 0.0f };
	static float gpu_time_data[res] = { 0.0f };
	static float cpu_histogram_max = FLT_MAX;
	static float gpu_histogram_max = FLT_MAX;

	double gpu_time_ms = _gpu_time * 1e-6;

	
	_cpu_time_min = min(_cpu_time_min, _cpu_time);
	_cpu_time_max = max(_cpu_time_max, _cpu_time);

	_gpu_time_min = min(_gpu_time_min, _gpu_time);
	_gpu_time_max = max(_gpu_time_max, _gpu_time);

	_frame_count++;
	_time_acc += delta_time;

	_cpu_time_acc += _cpu_time;
	_gpu_time_acc += _gpu_time;

	if (_time_acc > refresh_interval)
	{
		// Update CPU Timing Data
		_cpu_time_avg = _cpu_time_acc / _frame_count;
		if (draw_time_plot)
		{
			memmove(&cpu_time_data[0], &cpu_time_data[1], (res - 1) * sizeof(float));
			cpu_time_data[res - 1] = static_cast<float>(_cpu_time_avg);
		}
		_cpu_time_acc = 0;


		// Update GPU Timing Data
		_gpu_time_avg = _gpu_time_acc / _frame_count;
		if (draw_time_plot)
		{
			memmove(&gpu_time_data[0], &gpu_time_data[1], (res - 1) * sizeof(float));
			gpu_time_data[res - 1] = static_cast<float>(_gpu_time_avg * 1e-6);
		}
		_gpu_time_acc = 0;



		_time_acc = 0;
		_frame_count = 0;
	}

	if (ImGui::TreeNode("Scene Stats")) {
		//	ImGui::InputInt("Rendered Objects:", &_rendered_objects, 0, 0,
		//		ImGuiInputTextFlags_ReadOnly);
		ImGui::Checkbox("Draw Time plots", &draw_time_plot);
		ImGui::SliderScalar("Refresh Interval", ImGuiDataType_Double,
			&refresh_interval, &refresh_interval_min,
			&refresh_interval_max, "%.1f s");

		ImGui::InputDouble("CPU Time (ms)", &_cpu_time, 0, 0, "%.2f",
			ImGuiInputTextFlags_ReadOnly);
		ImGui::InputDouble("CPU Time Min (ms)", &_cpu_time_min, 0, 0, "%.2f",
			ImGuiInputTextFlags_ReadOnly);
		ImGui::InputDouble("CPU Time Max (ms)", &_cpu_time_max, 0, 0, "%.2f",
			ImGuiInputTextFlags_ReadOnly);
		ImGui::InputDouble("CPU Time Avg (ms)", &_cpu_time_avg, 0, 0, "%.2f",
			ImGuiInputTextFlags_ReadOnly);
		if (draw_time_plot)
		{
			ImGui::InputFloat("CPU Histogram Max", &cpu_histogram_max);


			if (cpu_histogram_max < 0) cpu_histogram_max = FLT_MAX;
			ImGui::Text("Avg CPU Time %.4f within %.1fs", _cpu_time_avg,
				refresh_interval);
			ImGui::PlotHistogram("CPU Times", cpu_time_data,
				IM_ARRAYSIZE(cpu_time_data), 0, nullptr, 0,
				cpu_histogram_max,
				ImVec2(0, 80));
		}

		ImGui::Checkbox("Show GPU Time in ms", &show_gpu_ms);
		if (show_gpu_ms)
		{
			double gpu_time_min_ms = _gpu_time_min * 1e-6;
			double gpu_time_max_ms = _gpu_time_max * 1e-6;
			double gpu_time_avg_ms = _gpu_time_avg * 1e-6;
			ImGui::InputDouble("GPU Time (ms)", &gpu_time_ms, 0, 0, "%.2f",
				ImGuiInputTextFlags_ReadOnly);
			ImGui::InputDouble("GPU Time Min (ms)", &gpu_time_min_ms, 0, 0, "%.2f",
				ImGuiInputTextFlags_ReadOnly);
			ImGui::InputDouble("GPU Time Max (ms)", &gpu_time_max_ms, 0, 0, "%.2f",
				ImGuiInputTextFlags_ReadOnly);
			ImGui::InputDouble("GPU Time Avg (ms)", &gpu_time_avg_ms, 0, 0, "%.2f",
				ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			ImGui::InputScalar("GPU Time (ns)", ImGuiDataType_U64, &_gpu_time, 0, 0, 0,
				ImGuiInputTextFlags_ReadOnly);
			ImGui::InputScalar("GPU Time Min (ns)", ImGuiDataType_U64, &_gpu_time_min,
				0, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputScalar("GPU Time Max (ns)", ImGuiDataType_U64, &_gpu_time_max,
				0, 0, 0, ImGuiInputTextFlags_ReadOnly);
			ImGui::InputScalar("GPU Time Avg (ns)", ImGuiDataType_U64, &_gpu_time_avg,
				0, 0, 0, ImGuiInputTextFlags_ReadOnly);
		}

		if (draw_time_plot)
		{
			ImGui::InputFloat("GPU Histogram Max", &gpu_histogram_max);
			if (gpu_histogram_max < 0) gpu_histogram_max = FLT_MAX;
			ImGui::Text("Avg GPU Time %.4f within %.1fs", _gpu_time_avg * 1e-6,
				refresh_interval);
			ImGui::PlotHistogram("GPU Times", gpu_time_data,
				IM_ARRAYSIZE(gpu_time_data), 0, nullptr, 0,
				gpu_histogram_max,
				ImVec2(0, 80));
		}
		ImGui::TreePop();
	}
}

void statUI(GlWindow* win)
{
	static int frame_count = 0;
	static float dt = 0.0f;
	static float fps = 0.0f;
	frame_count++;
	dt += _delta_time;

	if (dt > 1.0f)
	{
		fps = frame_count / dt;
		frame_count = 0;
		dt -= 1.0f;
	}

	float frame_time_ms = 1000.0f * static_cast<float>(_delta_time * 0.7);

	float mfps = fps;
	mfps += 20;

	ImGui::Text("General Stats:");
	//	ImGui::InputFloat("Frame Time (ms)", &frame_time_ms, 0, 0, "%.2f",
	//		ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("FPS", &mfps, 0, 0, "%.1f",
		ImGuiInputTextFlags_ReadOnly);
	ImGui::Separator();
	drawStatsImGui(win, _frame_start_time, _delta_time);

}

static void ShowInspectorOverlay(bool* p_open)
{
	ImGui::SetNextWindowPos(ImVec2(windowWidth - inspectorWidth, 22));
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::SetNextWindowSize(ImVec2(inspectorWidth, windowHeight));
	if (!ImGui::Begin("Inspector", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
		return;
	}

	statUI(win);
	ImGui::Separator();
	ImGui::Text("Crowd Simulation");
	
	if (ImGui::Button(u8">",ImVec2(40,40))) {
		
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"||", ImVec2(40, 40))) {

	}
	ImGui::SameLine();
	if (ImGui::Button(u8"ㅁ", ImVec2(40, 40))) {

	}


	ImGui::End();
}

/// <summary>
/// 하이어라키 화면
/// </summary>
/// <param name="p_open"></param>
static void ShowHierachyOverlay(bool* p_open)
{
	ImGui::SetNextWindowPos(ImVec2(0, 22));
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::SetNextWindowSize(ImVec2(200, windowHeight));

	if (!ImGui::Begin("Hierachy", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
		return;
	}

	ImGui::End();
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	win->setSize(windowWidth- inspectorWidth - hierachyWidth, windowHeight);
	win->setAspect((windowWidth-inspectorWidth-hierachyWidth) / (float)windowHeight);
}

/// <summary>
/// 
/// </summary>
/// <param name="p_open"></param>
static void ShowExampleAppMainMenuBar(bool* p_open)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Agent"))
		{
			ShowExampleMenuFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Obstacle"))
		{
			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Grid"))
		{
			winURI.printObj(win->getSelectedObj());
			if (ImGui::Button("Apply")) {
				win->update();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

static void ShowExampleMenuFile()
{
	if (ImGui::MenuItem("Agent Setting"))
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
	double halfW = screenWidth / 2;
	double halfH = screenHeight / 2;
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, _delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, _delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, _delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, _delta_time);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		cam.ProcessKeyboard(UPPER, _delta_time);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		cam.ProcessKeyboard(DOWN, _delta_time);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom -= (float)yoffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 1000.0f)
		zoom = 1000.0f;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

		cam.ProcessMouseMovement(xoffset, yoffset);
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

		cam.cameraPositionMove(xoffset, yoffset, _delta_time);
	}
	else {
		firstPosMouse = true;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
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

	window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL FrameWork", NULL, NULL);

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

	win = new GlWindow(screenWidth, screenHeight);

	// Timer Queries
	glGenQueries(2, _gpu_timer_query);
	// Dummy query
	glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.pong()]);
	glEndQuery(GL_TIME_ELAPSED);

	bool show_test_window = true;
	bool show_another_window = true;
	bool show_menubar = false;
	bool show_app_main_menu_bar = true;
	bool show_hierachy_window = true;

	ImVec4 clear_color = ImColor(144, 144, 144);

	_frame_start_time = glfwGetTime();

	float startFrame = glfwGetTime();


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

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	glfwSetWindowTitle(window, "CrowdSimulationFramework");
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		_delta_time = glfwGetTime() - _frame_start_time;
		_frame_start_time = glfwGetTime();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		_cpu_time = glfwGetTime();
		glBeginQuery(GL_TIME_ELAPSED, _gpu_timer_query[_swap_id.ping()]);

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static bool show_app_main_menu_bar = true;

		if (show_app_main_menu_bar) ShowExampleAppMainMenuBar(&show_app_main_menu_bar);

		if (show_another_window)
		{
			ShowInspectorOverlay(&show_another_window);
		}
		if (show_hierachy_window) {
			ShowHierachyOverlay(&show_hierachy_window);
		}

		static bool no_titlebar = false;

		ImGuiWindowFlags window_flags = 0;
		if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;

		win->draw(cam, zoom);

		glEndQuery(GL_TIME_ELAPSED);
		_cpu_time = (glfwGetTime() - _cpu_time) * 1000.0;
		glGetQueryObjectui64v(_gpu_timer_query[_swap_id.pong()], GL_QUERY_RESULT, &_gpu_time);
		_swap_id.swap();


		ImGui::Render();
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

	}

	//Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
