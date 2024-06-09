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
		// --- PushEnterを押したら画面に入ってくる ---
		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::TITLE)
			obj->state_++;

		break;
	}


	case 1:
	{
		auto* ui = obj->GetComponent<UIComponent>();
		auto* renderer = obj->GetComponent<SpriteRendererComponent>();

		RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.x, -1000.0f, 2.0f);

		renderer->color_.a = -ui->offset_.x / 1000.0f;

		obj->transform_->position_ = ui->offset_ + ui->basePosition_;

		Update(obj, renderer, elapsedTime);

		// --- ゲームが始まったら画面外へ ---
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

		if (RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.x, -1000.0f, 4.0f))
			obj->Destroy();

		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::TITLE)
			obj->Destroy();


		renderer->color_.a = -ui->offset_.x / 1000.0f;

		obj->transform_->position_ = ui->offset_ + ui->basePosition_;
	}

	}
}



void TutorialButtonBehavior::Update(GameObject* obj, SpriteRendererComponent* renderer, float elapsedTime)
{
	renderer->texPos_.y = renderer->texSize_.y * EventManager::Instance().button_.subEventIndex_;
}



void TitleLogoBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		auto* renderer = obj->GetComponent<SpriteRendererComponent>();

		// --- タイマーと三角関数を使って色を変化 ---
		obj->timer_ += 3.0f * elapsedTime;
		float color = sinf(obj->timer_);
		color = RootsLib::Math::Lerp(0.3f, 1.0f, (color + 1.0f) / 2.0f);
		renderer->color_.x = color;
		renderer->color_.y = color;
		renderer->color_.z = color;

		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::GAME)
		{
			auto* ui = obj->GetComponent<UIComponent>();
			ui->basePosition_ = obj->transform_->position_;

			obj->state_++;
		}

		break;
	}

	
	case 1:
	{
		auto* ui = obj->GetComponent<UIComponent>();
		auto* renderer = obj->GetComponent<SpriteRendererComponent>();

		if (RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.y, -600.0f, 1.0f))
			obj->Destroy();

		EventManager::Button& button = EventManager::Instance().button_;
		if (button.state_ == ButtonState::TITLE)
			obj->Destroy();

		obj->transform_->position_ = ui->offset_ + ui->basePosition_;


		break;
	}

	}
}
