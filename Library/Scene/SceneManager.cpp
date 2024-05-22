#include "SceneManager.h"


// --- シーンの変更 ---
void SceneManager::ChangeScene()
{
	// --- 次のシーンが異なるなら ---
	if (currentScene_ != nextScene_)
	{
		// --- 現在のシーンの終了処理 ---
		if (currentScene_)
			currentScene_->Finalize();

		// --- シーンを変更して初期設定 ---
		currentScene_ = nextScene_;

		if (!(nextScene_->IsReady()))
			currentScene_->Initialize();
	}
}


// --- 更新処理 ---
void SceneManager::Update(float elapsedTime)
{
	currentScene_->Update(elapsedTime);
}


// --- 描画処理 ---
void SceneManager::Draw(ID3D11DeviceContext* dc)
{
	currentScene_->Render(dc);
}
