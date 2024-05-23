#include "BehaviorManager.h"

#include "GameObject.h"

#include "CameraBehavior.h"

#include "../../Sources/Behavior/Player/PlayerBehavior.h"


BehaviorManager::BehaviorManager()
{
	behaviors_.insert(std::make_pair("DebugCamera", std::make_shared<DebugCameraBehavior>()));
	behaviors_.insert(std::make_pair("BasePlayer", std::make_shared<BasePlayerBehavior>()));
	behaviors_.insert(std::make_pair("PlayerController", std::make_shared<PlayerControllerBehavior>()));
}


BehaviorManager::~BehaviorManager()
{
	behaviors_.clear();
}


Behavior* BehaviorManager::GetBehavior(const char* key)
{
	return behaviors_.find(key)->second.get();
}
