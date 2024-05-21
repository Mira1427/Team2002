#include "BehaviorManager.h"

#include "GameObject.h"

#include "CameraBehavior.h"


BehaviorManager::BehaviorManager()
{
	behaviors_.insert(std::make_pair("DebugCamera", std::make_shared<DebugCameraBehavior>()));
}


BehaviorManager::~BehaviorManager()
{
	behaviors_.clear();
}


Behavior* BehaviorManager::GetBehavior(const char* key)
{
	return behaviors_.find(key)->second.get();
}
