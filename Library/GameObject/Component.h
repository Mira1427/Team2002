#pragma once

#include "GameObject.h"


class PlayerComponent final : public Component
{
public:
	PlayerComponent() :
		angleOffset_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float angleOffset_;
};



class PlayerControllerComponent final : public Component
{
public:
	PlayerControllerComponent() :
		range_(),
		rotateSpeed_(),
		actionGauge_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};
	void UpdateDebugGui(float elapsedTime) override;

	float range_;
	float rotateSpeed_;
	float actionGauge_;
};

