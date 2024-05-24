#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Component/Component.h"


class BasePlayerBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void Rotate(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller, float angleOffset);	// ��]����
	void Shot(GameObject* obj, PlayerComponent* player, PlayerControllerComponent* controller);	// �ˌ�����
	void AddBullet(const GameObject* parent, const float scaling);	// �e�ۂ̒ǉ�
};


// --- �v���C���[�̑��쏈�� ---
class PlayerControllerBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void Rotate(GameObject* obj, PlayerControllerComponent* controller, float elapsedTime) const;
};


// --- �v���C���[�̒e��Q�[�W�̏��� ---
class PlayerBulletGaugeBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};

