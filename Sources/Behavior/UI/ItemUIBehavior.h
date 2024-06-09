#pragma once

#include "../../Library/GameObject/GameObject.h"

class BaseItemUIBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	virtual void Update(GameObject* obj) {};
};


class ColorItemUIBehavior final : public BaseItemUIBehavior
{
private:
	void Update(GameObject* obj) override;
};

class GaugeItemUIBehavior final : public BaseItemUIBehavior
{
private:
	void Update(GameObject* obj) override;
};


class ItemUIBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};
};