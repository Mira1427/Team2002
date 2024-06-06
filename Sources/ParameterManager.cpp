#include "ParameterManager.h"


void ParameterManager::InitializeEffects()
{
	laserEffect_ = std::make_unique<Effect>("./Data/Effect/Laser/Laser.efk");
	smokeEffect_ = std::make_unique<Effect>("./Data/Effect/Smoke/Smoke.efk");
	spawnerSmokeEffect_ = std::make_unique<Effect>("./Data/Effect/Smoke/SpawnerSmoke.efk");
	explosionEffect_ = std::make_unique<Effect>("./Data/Effect/Explosion/Explosion.efk");
}
