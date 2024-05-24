#pragma once

#include "GameObject.h"


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

	float range_;			// キャラの距離
	float rotateSpeed_;		// 回転速度
	float actionGauge_;		// アクションゲージ
	float bullet_[2];		// 弾薬
	float addBulletValue_;	// 弾薬の増加量
	float maxBulletValue_;	// 弾薬の最大値
	float bulletCost_;		// 弾薬のコスト
};

