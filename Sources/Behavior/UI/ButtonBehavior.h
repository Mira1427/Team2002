#pragma once

#include "../../Library/GameObject/GameObject.h"

#include "../../Sources/EventManager.h"

class UIComponent;

class BaseButtonBehavior : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	virtual void Update(GameObject* obj, SpriteRendererComponent* renderer, UIComponent* ui) {};
	virtual bool IsSelected(GameObject* obj, EventManager::Button& button, UIComponent* ui);
	virtual void OnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime);
	virtual void OnUnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime);
};


class SubButtonBehavior final : public BaseButtonBehavior
{
private:
	void Update(GameObject* obj, SpriteRendererComponent* renderer, UIComponent* ui) override;
	bool IsSelected(GameObject* obj, EventManager::Button& button, UIComponent* ui) override { return false; }
	void OnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime) override {};
	void OnUnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime) override {};
};

class SceneBeginEraser final : public Eraser
{
public:
	void Execute(GameObject* obj) override;
};


// --- ポーズのボタン ---
class PauseButtonBehavior final : public BaseButtonBehavior
{
private:
	void Update(GameObject* obj, SpriteRendererComponent* renderer, UIComponent* ui) override {};
	void OnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime) override;
	void OnUnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime) override;
};