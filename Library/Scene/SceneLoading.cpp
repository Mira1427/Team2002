#include "SceneLoading.h"

#include <objbase.h>

#include "../Graphics/Graphics.h"
#include "../Graphics/TextureManager.h"

void SceneLoading::Initialize()
{
    texture_ = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/Load.png");

	CoInitialize(nullptr); // COM������
	std::thread loadThread([this]() { SceneLoading::LoadingThread(this); });
	loadThread.detach(); // �X���b�h�𕪗����āA���C���X���b�h�Ƃ͓Ɨ�������
}

void SceneLoading::Finalize()
{
    CoUninitialize(); // COM�I��
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


// --- ���[�f�B���O�X���b�h ---
void SceneLoading::LoadingThread(SceneLoading* scene)
{
    // scene�ւ̎Q�Ƃ𒼐ړn������ɁAscene�̃|�C���^���L���v�`�����Ďg�p
    if (scene->nextScene_) {
        scene->nextScene_->Initialize();
        scene->nextScene_->SetReady();
        SceneManager::Instance().SetNextScene(scene->nextScene_);
    }
}
