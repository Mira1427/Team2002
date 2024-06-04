#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Component/Component.h"


class BasePlayerBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void Rotate(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller, float angleOffset);	// 回転処理
	void Shot(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller);	// 射撃処理
	void AddBullet(GameObject* parent, const float scaling, const float attackAmount, const float radius, const float speed);	// 弾丸の追加
};


// --- プレイヤーの操作処理 ---
class PlayerControllerBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void Rotate(GameObject* obj, PlayerControllerComponent* controller, float elapsedTime) const;
	void ShotLaser(GameObject* obj, PlayerControllerComponent* controller);
	void AddLaser(GameObject* parent, const float attackAmount, const Vector3& size);
};


// --- プレイヤーの弾薬ゲージの処理 ---
class PlayerBulletGaugeBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

