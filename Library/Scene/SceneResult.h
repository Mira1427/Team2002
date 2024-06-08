#pragma once
#include "SceneManager.h"

class SceneResult : public Scene
{
public:
	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render(ID3D11DeviceContext* dc) override;

private:
	void ApplyBloom(ID3D11DeviceContext* dc);
};

