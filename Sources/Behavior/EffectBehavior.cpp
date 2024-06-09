#include "EffectBehavior.h"

#include "../../Library/Graphics/EffectManager.h"

#include "../EventManager.h"


void EffectBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (!EventManager::Instance().paused_)
		EffectManager::instance().update(elapsedTime);
}
