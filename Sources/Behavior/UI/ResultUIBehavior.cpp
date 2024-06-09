#include "ResultUIBehavior.h"

#include "../../Library/Audio/Audio.h"

#include "../../Library/Library/Library.h"

#include "../../Component/Component.h"

void ResultUIBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:
	{
		obj->timer_ -= elapsedTime;

		if (obj->timer_ < 0.0f)
			obj->state_++;

		break;
	}

	case 1:
	{
		auto* ui = obj->GetComponent<UIComponent>();
		if (RootsLib::Easing::Calculate(ui->easeData_, elapsedTime, ui->offset_.y, ui->easeScale_))
		{
			AudioManager::instance().playSound(4/*Result*/);
			obj->state_++;
		}

		obj->transform_->position_ = ui->basePosition_ + ui->offset_;
		break;
	}

	case 2:

		break;

	}
}
