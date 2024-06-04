#include "ParameterManager.h"


void ParameterManager::InitializeEffects()
{
	laserEffect_ = std::make_unique<Effect>("./Data/Effect/Laser/Laser.efk");
	smokeEffect_ = std::make_unique<Effect>("./Data/Effect/Smoke/Smoke.efk");
}
