#include "ButtonBehavior.h"

#include "../../Component/Component.h"


void BaseButtonBehavior::Execute(GameObject* obj, float elapsedTime)
{
	EventManager::Button& button = EventManager::Instance().button_;
	if (button.state_ != static_cast<ButtonState>(obj->state_))
		return;

	SpriteRendererComponent*	renderer = obj->GetComponent<SpriteRendererComponent>();
	UIComponent*				ui = obj->GetComponent<UIComponent>();
		
	Update(obj, renderer, ui);

	// --- ボタン選択時の処理 ---
	if (IsSelected(obj, button, ui))
	{
		OnSelected(obj, renderer, elapsedTime);
	}

	else
	{
		OnUnSelected(obj, renderer, elapsedTime);
	}
}


bool BaseButtonBehavior::IsSelected(GameObject* obj, EventManager::Button& button, UIComponent* ui)
{
	return button.eventIndex_ == ui->eventID_;
}

void BaseButtonBehavior::OnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime)
{
	renderer->color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void BaseButtonBehavior::OnUnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime)
{
	renderer->color_ = { 0.5f, 0.5f, 0.5f, 1.0f };
}


void SceneBeginEraser::Execute(GameObject* obj)
{
	if (EventManager::Instance().button_.state_ != ButtonState::SCENE_BEGIN)
		obj->Destroy();
}



void SubButtonBehavior::Update(GameObject* obj, SpriteRendererComponent* renderer, UIComponent* ui)
{
	renderer->texPos_.y = renderer->texSize_.y * EventManager::Instance().button_.subEventIndex_;
}



// --- ポーズのボタン ---
void PauseButtonBehavior::OnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime)
{
	renderer->texPos_.y = renderer->texSize_.y * 0.0f;
}

void PauseButtonBehavior::OnUnSelected(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime)
{
	renderer->texPos_.y = renderer->texSize_.y * 1.0f;
}
