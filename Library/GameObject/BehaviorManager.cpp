#include "BehaviorManager.h"

#include "GameObject.h"

#include "CameraBehavior.h"

#include "../../Sources/Behavior/Player/PlayerBehavior.h"
#include "../../Sources/Behavior/Player/BulletBehavior.h"
#include "../../Sources/Behavior/Player/AttackGaugeBehavior.h"

#include "../../Sources/Behavior/Enemy/EnemyBehavior.h"
#include "../../Sources/Behavior/Enemy/EnemySpawnerBehavior.h"


BehaviorManager::BehaviorManager()
{
	behaviors_.insert(std::make_pair("DebugCamera",			std::make_shared<DebugCameraBehavior>()));

	behaviors_.insert(std::make_pair("BasePlayer",			std::make_shared<BasePlayerBehavior>()));
	behaviors_.insert(std::make_pair("PlayerController",	std::make_shared<PlayerControllerBehavior>()));
	behaviors_.insert(std::make_pair("BulletGauge",			std::make_shared<PlayerBulletGaugeBehavior>()));
	behaviors_.insert(std::make_pair("Bullet",				std::make_shared<BaseBulletBehavior>()));
	behaviors_.insert(std::make_pair("AttackGauge",			std::make_shared<AttackGaugeBehavior>()));
	behaviors_.insert(std::make_pair("RangeGauge",			std::make_shared<RangeGaugeBehavior>()));

	behaviors_.insert(std::make_pair("BaseEnemy",			std::make_shared<BaseEnemyBehavior>()));
	behaviors_.insert(std::make_pair("EnemySpawner",		std::make_shared<EnemySpawnerBehavior>()));
}


BehaviorManager::~BehaviorManager()
{
	behaviors_.clear();
}


Behavior* BehaviorManager::GetBehavior(const char* key)
{
	return behaviors_.find(key)->second.get();
}
