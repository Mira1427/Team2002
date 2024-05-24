#pragma once

#include "../../Library/GameObject/GameObject.h"


class SceneEraser final : public Eraser
{
public:
	void Execute(GameObject* obj) override;
};