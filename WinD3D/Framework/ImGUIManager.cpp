#include "ImGUIManager.h"
#include "../ImGUI/imgui.h"

ImGUIManager::ImGUIManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
}
ImGUIManager::~ImGUIManager()
{
	ImGui::DestroyContext();
}
