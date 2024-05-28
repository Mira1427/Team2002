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
		attackGauge_(),
		addAttackGaugeValue_(),
		bullet_(),
		addBulletValue_(),
		maxBulletValue_(),
		bulletCost_(),
		attackAmount_(),
		rangeAmount_(),
		maxAttackAmount_(),
		minAttackAmount_(),
		maxRangeAmount_(),
		minRangeAmount_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float range_;				// �L�����̋���
	float rotateSpeed_;			// ��]���x

	float attackGauge_;			// �U���Q�[�W
	float addAttackGaugeValue_;	// �U���Q�[�W�̑�����

	float bullet_[2];			// �e��
	float addBulletValue_;		// �e��̑�����
	float maxBulletValue_;		// �e��̍ő�l
	float bulletCost_;			// �e��̃R�X�g

	float attackAmount_;	// �U����
	float rangeAmount_;		// �͈�
	float maxAttackAmount_;		// �U���̍ő�l
	float minAttackAmount_;		// �U���̍ŏ��l
	float maxRangeAmount_;		// �͈͂̍ő�l
	float minRangeAmount_;		// �͈͂̍ŏ��l
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


enum class EnemyType
{
	WHITE,
	BLACK,
	GRAY
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
	EnemyType type_;
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


// --- �e�̃R���|�[�l���g ---
class BulletComponent final : public Component
{
public:
	BulletComponent() :
		attack_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float attack_;
};