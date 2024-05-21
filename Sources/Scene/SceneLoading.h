#pragma once

#include <thread>

#include "SceneManager.h"


struct Texture;

// ===== ���[�f�B���O ======================================================================================================================================================
class SceneLoading final : public Scene
{
public:
	SceneLoading(std::shared_ptr<Scene> nextScene) : nextScene_(nextScene) {}

	// --- ������ ---
	void Initialize() override;

	// --- �I������ ---
	void Finalize() override;

	// --- �X�V���� ---
	void Update(float elapsedTime) override;

	// --- �`�揈�� ---
	void Render(ID3D11DeviceContext* dc) override;

private:
	static void LoadingThread(SceneLoading* scene);

	std::shared_ptr<Scene> nextScene_;
	std::thread* thread_ = nullptr;

	Texture* texture_;
};

