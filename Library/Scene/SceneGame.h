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
	GameObject* AddPlayer(std::string name, GameObject* parent, float rotate, float range, int playerNum);

	// --- �v���C���[�̃R���g���[���[�̒ǉ� ---
	GameObject* AddPlayerController(float rotateSpeed, float range);

	// --- �X�e�[�W�̒ǉ� ---
	void AddStage();

	// --- �e��Q�[�W�̒ǉ� ---
	void AddBulletGauge(GameObject* parent, int i);

	// --- �X�|�i�[�̒ǉ� ---
	void AddEnemySpawner();
};

