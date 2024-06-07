#include "BehaviorManager.h"

#include "GameObject.h"

#include "CameraBehavior.h"

#include "../../Library/GameObject/ParticleBehavior.h"

#include "../../Sources/Behavior/Player/PlayerBehavior.h"
#include "../../Sources/Behavior/Player/BulletBehavior.h"
#include "../../Sources/Behavior/Player/LaserBehavior.h"
#include "../../Sources/Behavior/Player/AttackGaugeBehavior.h"

#include "../../Sources/Behavior/Enemy/EnemyBehavior.h"
#include "../../Sources/Behavior/Enemy/EnemySpawnerBehavior.h"

#include "../../Sources/Behavior/UI/PauseBehavior.h"
#include "../../Sources/Behavior/UI/LifeGaugeBehavior.h"
#include "../../Sources/Behavior/UI/WaveCutInBehavior.h"
#include "../../Sources/Behavior/UI/ButtonBehavior.h"
#include "../../Sources/Behavior/UI/ItemUIBehavior.h"

#include "../../Sources/Behavior/GameCamera.h"


BehaviorManager::BehaviorManager()
{
	behaviors_.insert(std::make_pair("DebugCamera",			std::make_shared<DebugCameraBehavior>()));
	behaviors_.insert(std::make_pair("GameCamera",			std::make_shared<GameCameraBehavior>()));

	behaviors_.insert(std::make_pair("Particle",			std::make_shared<ParticleBehavior>()));

	behaviors_.insert(std::make_pair("BasePlayer",			std::make_shared<BasePlayerBehavior>()));
	behaviors_.insert(std::make_pair("PlayerController",	std::make_shared<PlayerControllerBehavior>()));
	behaviors_.insert(std::make_pair("BulletGauge",			std::make_shared<PlayerBulletGaugeBehavior>()));
	behaviors_.insert(std::make_pair("Bullet",				std::make_shared<BaseBulletBehavior>()));
	behaviors_.insert(std::make_pair("BulletExplosion",		std::make_shared<BulletExplosionBehavior>()));
	behaviors_.insert(std::make_pair("Laser",				std::make_shared<LaserBehavior>()));
	behaviors_.insert(std::make_pair("AttackGauge",			std::make_shared<AttackGaugeBehavior>()));
	behaviors_.insert(std::make_pair("RangeGauge",			std::make_shared<RangeGaugeBehavior>()));
	behaviors_.insert(std::make_pair("AttackGaugeBar",		std::make_shared<AttackGaugeBarBehavior>()));
	behaviors_.insert(std::make_pair("ColorItemUI",			std::make_shared<ColorItemUIBehavior>()));
	behaviors_.insert(std::make_pair("GaugeItemUI",			std::make_shared<GaugeItemUIBehavior>()));

	behaviors_.insert(std::make_pair("BaseEnemy",			std::make_shared<BaseEnemyBehavior>()));
	behaviors_.insert(std::make_pair("FlyEnemy",			std::make_shared<FlyEnemyBehavior>()));
	behaviors_.insert(std::make_pair("EnemySpawner",		std::make_shared<EnemySpawnerBehavior>()));

	behaviors_.insert(std::make_pair("PauseOverlay",		std::make_shared<PauseOverlayBehavior>()));
	behaviors_.insert(std::make_pair("LifeGauge",			std::make_shared<LifeGaugeBehavior>()));
	behaviors_.insert(std::make_pair("WaveCutIn",			std::make_shared<WaveCutInBehavior>()));
	behaviors_.insert(std::make_pair("BaseButton",			std::make_shared<BaseButtonBehavior>()));
}


BehaviorManager::~BehaviorManager()
{
	behaviors_.clear();
}


Behavior* BehaviorManager::GetBehavior(const char* key)
{
	return behaviors_.find(key)->second.get();
}
