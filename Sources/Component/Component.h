#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Library/Library/Library.h"



enum class CharacterType
{
	WHITE,
	BLACK,
	GRAY
};

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
	CharacterType type_;
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
		minRangeAmount_(),
		maxAttackGaugeHeight_(),
		shotLaser_(),
		laserAttackAmount_(),
		laserSize_()
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
	float maxAttackAmount_;	// �U���̍ő�l
	float minAttackAmount_;	// �U���̍ŏ��l
	float maxRangeAmount_;	// �͈͂̍ő�l
	float minRangeAmount_;	// �͈͂̍ŏ��l
	float maxAttackGaugeHeight_;

	bool  shotLaser_;
	float laserAttackAmount_;
	Vector3 laserSize_;
};


// --- �G�̃X�|�i�[�̃R���|�[�l���g ---
class EnemySpawnerComponent final : public Component
{
public:
	EnemySpawnerComponent() :
		spawnSpeed_(),
		spawnCount_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float spawnSpeed_;
	int spawnCount_;
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
	CharacterType type_;
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
		attack_(),
		radius_(),
		type_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float attack_;
	float radius_;
	CharacterType type_;
};



class UIComponent final : public Component
{
public:
	UIComponent() :
		offset_(),
		uniqueID_(-1),
		basePosition_(),
		easeScale_(),
		easeData_(1.0f, false, nullptr)
	{}


	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	Vector3 offset_;
	int uniqueID_;
	Vector3 basePosition_;
	float easeScale_;
	RootsLib::Easing::Data easeData_;
};