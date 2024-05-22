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

	void Initialize();	// 初期化
	void Finalize();	// 終了処理
	void Update();		// 更新処理
	void Draw();		// 描画処理

	void SetStyle();	// 見た目の変更
};

