#include "ImGuiManager.h"
#include "ImGui/imgui.h"
#include <filesystem>

ImGuiManager::ImGuiManager()
{
	
	namespace fs = std::filesystem;
	if (!fs::exists("imgui.ini") && fs::exists("imgui_default.ini"))
	{
		fs::copy_file("imgui_default.ini", "imgui.ini");
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiCol_DockingPreview;

	ImGui::StyleColorsDark();
}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}
