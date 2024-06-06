#include "SceneLoading.h"

#include <objbase.h>

#include "../Graphics/Graphics.h"
#include "../Graphics/TextureManager.h"

void SceneLoading::Initialize()
{
    texture_ = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/Load.png");

	CoInitialize(nullptr); // COM初期化
	std::thread loadThread([this]() { SceneLoading::LoadingThread(this); });
	loadThread.detach(); // スレッドを分離して、メインスレッドとは独立させる
}

void SceneLoading::Finalize()
{
    CoUninitialize(); // COM終了
}

void SceneLoading::Update(float elapsedTime)
{
}

void SceneLoading::Render(ID3D11DeviceContext* dc)
{
    Graphics::Instance().GetSpriteRenderer()->Draw(
        Graphics::Instance().GetDeviceContext(),
        *texture_,
        Vector3::Zero_,
        Vector3::Unit_,
        Vector2::Zero_,
        { 1280.0f,720.0f }
    );
}


// --- ローディングスレッド ---
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // sceneへの参照を直接渡す代わりに、sceneのポインタをキャプチャして使用
    if (scene->nextScene_) {
        scene->nextScene_->Initialize();
        scene->nextScene_->SetReady();
        SceneManager::Instance().SetNextScene(scene->nextScene_);
    }
}
