#pragma once

#include "ButtonBehavior.h"

class TitleUIBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	virtual void Update(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime) {};
};


class TutorialButtonBehavior final : public TitleUIBehavior
{
private:
	void Update(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime) override;
};

