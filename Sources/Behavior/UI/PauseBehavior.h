#pragma once

#include "../../Library/GameObject/GameObject.h"


class PauseOverlayBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};


class PauseEraser final : public Eraser
{
public:
	void Execute(GameObject* obj) override;
};

