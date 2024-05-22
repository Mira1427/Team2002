#pragma once

#include "GameObject.h"


// --- カメラ処理の基底クラス ---
class BaseCameraBehavior : public Behavior
{
public:
	virtual void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};


// --- デバッグのカメラ処理 ---
class DebugCameraBehavior : public BaseCameraBehavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;

private:
	void ApplyControl(GameObject* obj, float elapsedTime);
};