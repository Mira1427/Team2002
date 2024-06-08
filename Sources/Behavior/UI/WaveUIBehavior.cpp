#include "WaveUIBehavior.h"

#include "../../Library/Math/Easing.h"

#include "../../Component/Component.h"

#include "../../EventManager.h"
#include "../../ParameterManager.h"


void WaveCutInBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:

	{
		UIComponent* ui = obj->GetComponent<UIComponent>();
		ui->easeData_.function_ = RootsLib::Easing::GetFunction(EaseOutCubic);
		ui->basePosition_ = { 1500.0f, 360.0f, 0.0f };
		ui->easeScale_ = 1500.0f - 640.0f;
		ui->easeData_.timerLimit_ = 0.5f;
	}

		obj->state_++;
		[[fallthrough]];

	case 1:

	{
		UIComponent* ui = obj->GetComponent<UIComponent>();

		if (RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.x, -ui->easeScale_))
		{
			obj->state_++;
		}

		obj->transform_->position_ = ui->basePosition_ + ui->offset_;
	}

		break;


	case 2:

	{
		obj->timer_ += elapsedTime;

		if (obj->timer_ > 1.0f)
		{
			UIComponent* ui = obj->GetComponent<UIComponent>();

			ui->easeData_.function_ = RootsLib::Easing::GetFunction(EaseInCubic);
			ui->easeData_.timer_ = 0.0f;
			ui->basePosition_ = obj->transform_->position_;
			ui->easeScale_ = 1000.0f;

			obj->state_++;
		}
	}

		break;


	case 3:

	{
		UIComponent* ui = obj->GetComponent<UIComponent>();

		if (RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.x, -ui->easeScale_))
		{
			obj->Destroy();
		}

		obj->transform_->position_ = ui->basePosition_ + ui->offset_;
	}

		break;

	}
}

void WaveUIBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
		obj->state_++;
		[[fallthrough]];

	case 1:

	{
		auto* renderer = obj->GetComponent<SpriteRendererComponent>();

		// --- 描画位置とサイズの設定 ---
		int wave = EventManager::Instance().enemySpawner_->state_ / 2;	// 現在のウェーブ
		if (wave > 10)
			return;

		renderer->texSize_.x = (wave != 10) ? 330.0f : TextureManager::Instance().GetTexture(L"./Data/Texture/UI/wave.png")->width_;
		renderer->texPos_.y = renderer->texSize_.y * (wave - 1);
	}
		break;
	}
}
