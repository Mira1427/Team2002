#include "SceneManager.h"


// --- �V�[���̕ύX ---
void SceneManager::ChangeScene()
{
	// --- ���̃V�[�����قȂ�Ȃ� ---
	if (currentScene_ != nextScene_)
	{
		// --- ���݂̃V�[���̏I������ ---
		if (currentScene_)
			currentScene_->Finalize();

		// --- �V�[����ύX���ď����ݒ� ---
		currentScene_ = nextScene_;

		if (!(nextScene_->IsReady()))
			currentScene_->Initialize();
	}
}


// --- �X�V���� ---
void SceneManager::Update(float elapsedTime)
{
	currentScene_->Update(elapsedTime);
}


// --- �`�揈�� ---
void SceneManager::Draw(ID3D11DeviceContext* dc)
{
	currentScene_->Render(dc);
}
