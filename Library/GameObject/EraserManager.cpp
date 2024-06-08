#include "EraserManager.h"

#include "GameObject.h"

#include "../../Sources/Eraser/SceneEraser.h"

#include "../../Sources/Behavior/UI/PauseBehavior.h"
#include "../../Sources/Behavior/UI/ButtonBehavior.h"


EraserManager::EraserManager()
{
	erasers_.insert(std::make_pair("Scene", std::make_shared<SceneEraser>()));

	erasers_.insert(std::make_pair("Pause", std::make_shared<PauseEraser>()));
	erasers_.insert(std::make_pair("SceneBegin", std::make_shared<SceneBeginEraser>()));
}


EraserManager::~EraserManager()
{
	erasers_.clear();
}


Eraser* EraserManager::GetEraser(const char* key)
{
	return erasers_.find(key)->second.get();
}
