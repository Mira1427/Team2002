#include "TitleUIBehavior.h"

#include "../../Library/Math/Easing.h"

#include "../../Library/Library/Library.h"

#include "../../Component/Component.h"


void TitleUIBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		// --- PushEnter‚ð‰Ÿ‚µ‚½‚ç‰æ–Ê‚É“ü‚Á‚Ä‚­‚é ---
		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::TITLE)
			obj->state_++;

		break;
	}


	case 1:
	{
		auto* ui = obj->GetComponent<UIComponent>();
		auto* renderer = obj->GetComponent<SpriteRendererComponent>();

		RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.x, -600.0f, 2.0f);

		renderer->color_.a = -ui->offset_.x / 600.0f;

		obj->transform_->position_ = ui->offset_ + ui->basePosition_;

		Update(obj, renderer, elapsedTime);

		// --- ƒQ[ƒ€‚ªŽn‚Ü‚Á‚½‚ç‰æ–ÊŠO‚Ö ---
		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::GAME)
		{
			ui->offset_.x = obj->transform_->position_.x;
			ui->easeData_.isReverse_ = true;
			obj->state_++;
		}

		break;
	}

	
	case 2:
	{
		auto* ui = obj->GetComponent<UIComponent>();
		auto* renderer = obj->GetComponent<SpriteRendererComponent>();

		if (RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.x, -600.0f, 4.0f))
			obj->Destroy();

		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::TITLE)
			obj->Destroy();


		renderer->color_.a = -ui->offset_.x / 600.0f;

		obj->transform_->position_ = ui->offset_ + ui->basePosition_;
	}

	}
}



void TutorialButtonBehavior::Update(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime)
{
	renderer->texPos_.y = renderer->texSize_.y * EventManager::Instance().button_.subEventIndex_;
}
