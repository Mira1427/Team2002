#pragma once

#include "SceneManager.h"

#include <string>


class GameObject;

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

	// --- �u���[���̓K�p ---
	void ApplyBloom(ID3D11DeviceContext* dc);

	// --- �v���C���[�̒ǉ� ---
	void AddPlayer(std::string name, GameObject* parent, float rotate, float range);

	// --- �v���C���[�̃R���g���[���[�̒ǉ� ---
	GameObject* AddPlayerController(float rotateSpeed, float range);
};

