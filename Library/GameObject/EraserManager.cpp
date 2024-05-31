#include "EraserManager.h"

#include "GameObject.h"

#include "../../Sources/Eraser/SceneEraser.h"

#include "../../Sources/Behavior/UI/PauseBehavior.h"


EraserManager::EraserManager()
{
	erasers_.insert(std::make_pair("Scene", std::make_shared<SceneEraser>()));

	erasers_.insert(std::make_pair("Pause", std::make_shared<PauseEraser>()));
}


EraserManager::~EraserManager()
{
	erasers_.clear();
}


Eraser* EraserManager::GetEraser(const char* key)
{
	return erasers_.find(key)->second.get();
}
