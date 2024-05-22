#pragma once

#include "SceneManager.h"


// ===== �Q�[�� ======================================================================================================================================================
class SceneGame final : public Scene
{
public:
	// --- ������ ---
	void Initialize() override;

	// --- �I������ ---
	void Finalize() override;

	// --- �X�V���� ---
	void Update(float elapsedTime) override;

	// --- �`�揈�� ---
	void Render(ID3D11DeviceContext* dc) override;
};

