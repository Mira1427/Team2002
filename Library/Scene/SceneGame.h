#pragma once

#include "SceneManager.h"


// ===== ゲーム ======================================================================================================================================================
class SceneGame final : public Scene
{
public:
	// --- 初期化 ---
	void Initialize() override;

	// --- 終了処理 ---
	void Finalize() override;

	// --- 更新処理 ---
	void Update(float elapsedTime) override;

	// --- 描画処理 ---
	void Render(ID3D11DeviceContext* dc) override;
};

