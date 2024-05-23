#include "ImGuiManager.h"

#include "../Library/Library.h"


// --- 初期化 ---
void ImGuiManager::Initialize()
{
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\YuGothM.ttc", 14.0f, nullptr, glyphRangesJapanese);
	ImGui_ImplWin32_Init(RootsLib::Window::GetHandle());
	ImGui_ImplDX11_Init(RootsLib::DX11::GetDevice(), RootsLib::DX11::GetDeviceContext());
	ImGui::StyleColorsDark();

	SetStyle();
#endif
}


// --- 終了処理 ---
void ImGuiManager::Finalize()
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}


// --- 更新処理 ---
void ImGuiManager::Update()
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 10.0f);
#endif
}


// --- 描画処理 ---
void ImGuiManager::Draw()
{
#ifdef USE_IMGUI
	ImGui::PopStyleVar(3);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}


// --- 見た目の変更 ---
void ImGuiManager::SetStyle()
{
#ifdef USE_IMGUI
	auto& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TextDisabled] = ImVec4{ 0.50f, 0.50f, 0.50f, 0.50f };
	style.Colors[ImGuiCol_WindowBg] = ImVec4{ 0.30f, 0.30f, 0.30f, 0.30f };
	style.Colors[ImGuiCol_ChildBg] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PopupBg] = ImVec4{ 0.08f, 0.08f, 0.08f, 1.00f };
	style.Colors[ImGuiCol_Border] = ImVec4{ 0.70f, 0.70f, 1.00f, 0.00f };
	style.Colors[ImGuiCol_BorderShadow] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.00f };
	style.Colors[ImGuiCol_FrameBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.30f };
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.25f };
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_TitleBg] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.00f, 0.00f, 0.00f, 1.00f };
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.50f };
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.10f, 0.10f, 0.10f, 0.80f };
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.50f, 0.50f, 0.50f, 0.50f };
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.40f, 0.40f, 0.40f, 1.00f };
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.80f };
	style.Colors[ImGuiCol_CheckMark] = ImVec4{ 0.26f, 0.59f, 0.98f, 1.00f };
	style.Colors[ImGuiCol_SliderGrab] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.40f };
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.70f };
	style.Colors[ImGuiCol_Button] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.15f };
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.50f };
	style.Colors[ImGuiCol_Header] = ImVec4{ 0.30f, 0.60f, 1.00f, 0.50f };
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.15f };
	style.Colors[ImGuiCol_HeaderActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.30f };
	style.Colors[ImGuiCol_Separator] = ImVec4{ 0.70f, 0.70f, 0.70f, 1.00f };
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.25f };
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 0.60f };
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_Tab] = ImVec4{ 0.00f, 0.00f, 0.00f, 0.30f };
	style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.40f, 0.40f, 0.40f, 0.60f };
	style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.50f, 0.50f, 0.50f, 1.00f };
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotLines] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4{ 0.50f, 0.50f, 0.50f, 0.60f };
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4{ 1.00f, 1.00f, 1.00f, 1.00f };
#endif
}
