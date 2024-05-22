#pragma once

#include <thread>

#include "SceneManager.h"


struct Texture;

// ===== ローディング ======================================================================================================================================================
class SceneLoading final : public Scene
{
public:
	SceneLoading(std::shared_ptr<Scene> nextScene) : nextScene_(nextScene) {}

	// --- 初期化 ---
	void Initialize() override;

	// --- 終了処理 ---
	void Finalize() override;

	// --- 更新処理 ---
	void Update(float elapsedTime) override;

	// --- 描画処理 ---
	void Render(ID3D11DeviceContext* dc) override;

private:
	static void LoadingThread(SceneLoading* scene);

	std::shared_ptr<Scene> nextScene_;
	std::thread* thread_ = nullptr;

	Texture* texture_;
};

