#ifndef _IMGUI_USER_RENDERING_INTERFACE_
#define _IMGUI_USER_RENDERING_INTERFACE_

#include"imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imguifilesystem.h"

#include"Component.h"
#include"Model/Grid.h"

class imguiURI {
public:
	void printObj(grid* grid);
	void printTransform(Transform trns);
	
};


#endif