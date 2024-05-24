#pragma once

#include "../../Library/GameObject/GameObject.h"


// --- �v���C���[�̃R���|�[�l���g ---
class PlayerComponent final : public Component
{
public:
	PlayerComponent() :
		angleOffset_(),
		playerNum_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float angleOffset_;
	int playerNum_;
};


// --- �v���C���[����̃R���|�[�l���g ---
class PlayerControllerComponent final : public Component
{
public:
	PlayerControllerComponent() :
		range_(),
		rotateSpeed_(),
		actionGauge_(),
		bullet_(),
		addBulletValue_(1.0f),
		maxBulletValue_(),
		bulletCost_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float range_;			// �L�����̋���
	float rotateSpeed_;		// ��]���x
	float actionGauge_;		// �A�N�V�����Q�[�W
	float bullet_[2];		// �e��
	float addBulletValue_;	// �e��̑�����
	float maxBulletValue_;	// �e��̍ő�l
	float bulletCost_;		// �e��̃R�X�g
};


// --- �G�̃X�|�i�[�̃R���|�[�l���g ---
class EnemySpawnerComponent final : public Component
{
public:
	EnemySpawnerComponent() :
		spawnSpeed_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float spawnSpeed_;
};


// --- �G�̃R���|�[�l���g ---
class EnemyComponent final : public Component
{
public:
	EnemyComponent() :
		life_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float life_;
};


// --- �X�e�[�W�̃R���|�[�l���g ---
class StageComponent final : public Component
{
public:
	StageComponent() :
		life_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float life_;
};