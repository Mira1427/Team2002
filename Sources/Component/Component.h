#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Library/Library/Library.h"



enum class CharacterType
{
	WHITE,
	BLACK,
	GRAY
};

// --- プレイヤーのコンポーネント ---
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


// --- プレイヤー操作のコンポーネント ---
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

	float range_;				// キャラの距離
	float rotateSpeed_;			// 回転速度

	float attackGauge_;			// 攻撃ゲージ
	float addAttackGaugeValue_;	// 攻撃ゲージの増加量

	float bullet_[2];			// 弾薬
	float addBulletValue_;		// 弾薬の増加量
	float maxBulletValue_;		// 弾薬の最大値
	float bulletCost_;			// 弾薬のコスト

	float attackAmount_;	// 攻撃力
	float rangeAmount_;		// 範囲
	float maxAttackAmount_;	// 攻撃の最大値
	float minAttackAmount_;	// 攻撃の最小値
	float maxRangeAmount_;	// 範囲の最大値
	float minRangeAmount_;	// 範囲の最小値
	float maxAttackGaugeHeight_;

	bool  shotLaser_;
	float laserAttackAmount_;
	Vector3 laserSize_;
};


// --- 敵のスポナーのコンポーネント ---
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

// --- 敵のコンポーネント ---
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


// --- ステージのコンポーネント ---
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


// --- 弾のコンポーネント ---
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