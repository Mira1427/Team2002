#pragma once

#include "SceneManager.h"

#include <string>


class GameObject;

class Vector3;

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

	// --- ���[���̒ǉ� ---
	void AddRail();

	// --- �e��Q�[�W�̒ǉ� ---
	void AddBulletGauge(GameObject* parent, int i);

	// --- �X�|�i�[�̒ǉ� ---
	void AddEnemySpawner();

	// --- �X�̃��C�t�ǉ� ---
	void AddTownLife(const int life, const float range);

	// --- ���C�t�Q�[�W�̒ǉ� ---
	void AddLifeGauge();

	// --- �U���Q�[�W�̃R���g���[���[�ǉ� ---
	GameObject* AddAttackGaugeController(GameObject* parent, const Vector3& position);

	// --- �U���Q�[�W�̒ǉ� ---
	void AddAttackGauge(GameObject* parent, float width);

	// --- �͈̓Q�[�W�̒ǉ� ---
	void AddRangeGauge(GameObject* parent, float width);

	// --- �U���Q�[�W�̃o�[�̒ǉ� ---
	void AddAttackGaugeBar(GameObject* parent);
};

