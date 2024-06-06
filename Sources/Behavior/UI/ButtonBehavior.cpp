#include "ButtonBehavior.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"


void BaseButtonBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		[[fallthrough]];

	case 1:

	{
		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ != static_cast<ButtonState>(obj->state_))
			return;

		SpriteRendererComponent*	renderer = obj->GetComponent<SpriteRendererComponent>();
		UIComponent*				ui = obj->GetComponent<UIComponent>();
		
		// --- ƒ{ƒ^ƒ“‘I‘ðŽž‚Ìˆ— ---
		if (button.eventIndex_ == ui->eventID_)
		{
			renderer->color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
		}

		else
		{
			renderer->color_ = { 0.5f, 0.5f, 0.5f, 1.0f };
		}
	}

		break;
	}
}
