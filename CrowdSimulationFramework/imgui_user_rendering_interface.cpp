#include"imgui_user_rendering_interface.h"

//추 후 object 상위 클래스 작성 및 상속 필요
void imguiURI::printObj(grid* obj) {
	printTransform(obj->myTrns);
	if (obj->tag == "grid") {
		ImGui::Separator();
		ImGui::InputInt("gridSize", &obj->gridSize,25);
		ImGui::Separator();
		ImGui::InputInt("gridNum", &obj->gridNum,5);
	}
}

void imguiURI::printTransform(Transform trns) {
	ImGui::Text("Transform");
	ImGui::Separator();
	ImGui::InputFloat3("Position", trns.Position);
	ImGui::Separator();
	ImGui::InputFloat3("Rotation", trns.Rotation);
	ImGui::Separator();
	ImGui::InputFloat3("Scale", trns.Scale);
}