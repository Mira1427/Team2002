#pragma once

#ifdef USE_IMGUI
#include "../../External/ImGui/imgui.h"
#include "../../External/ImGui/imgui_internal.h"
#include "../../External/ImGui/imgui_impl_dx11.h"
#include "../../External/ImGui/imgui_impl_win32.h"
#include "../../External/ImGui/imstb_truetype.h"
extern ImWchar glyphRangesJapanese[];
#endif

class ImGuiManager
{
private:
	ImGuiManager() {}
	~ImGuiManager() {}

public:
	static ImGuiManager& Instance()
	{
		static ImGuiManager instance;
		return instance;
	}

	void Initialize();	// ������
	void Finalize();	// �I������
	void Update();		// �X�V����
	void Draw();		// �`�揈��

	void SetStyle();	// �����ڂ̕ύX
};

