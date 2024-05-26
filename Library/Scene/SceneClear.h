#pragma once

#include "SceneManager.h"


class SceneClear final : public Scene
{
public:
	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render(ID3D11DeviceContext* dc) override;
};

