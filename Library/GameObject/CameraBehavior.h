#pragma once

#include "GameObject.h"


// --- �J���������̊��N���X ---
class BaseCameraBehavior : public Behavior
{
public:
	virtual void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};


// --- �f�o�b�O�̃J�������� ---
class DebugCameraBehavior : public BaseCameraBehavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;

private:
	void ApplyControl(GameObject* obj, float elapsedTime);
};