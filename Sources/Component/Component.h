#pragma once

#include "../../Library/GameObject/GameObject.h"


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
		minRangeAmount_()
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
	float maxAttackAmount_;		// 攻撃の最大値
	float minAttackAmount_;		// 攻撃の最小値
	float maxRangeAmount_;		// 範囲の最大値
	float minRangeAmount_;		// 範囲の最小値
};


// --- 敵のスポナーのコンポーネント ---
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
	EnemyType type_;
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
		attack_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float attack_;
};