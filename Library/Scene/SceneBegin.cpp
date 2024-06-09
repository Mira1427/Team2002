#include "SceneBegin.h"

#include "../Audio/Audio.h"

#include "SceneTitle.h"

#include "../../Sources/EventManager.h"

void SceneBegin::Initialize()
{
	EventManager::Instance().InitializeObjects();


	{
		// --- âπäyì«Ç›çûÇ› ---
		AudioManager::instance().LoadMusic("./Data/Music/Title.wav");
		AudioManager::instance().LoadMusic("./Data/Music/Game.wav");

		// --- å¯â âπì«Ç›çûÇ› ---
		AudioManager::instance().LoadSound("./Data/Sound/Beam.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Damage.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Item.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Kill.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Result.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Shot_White.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Shot_Black.wav");
		AudioManager::instance().LoadSound("./Data/Sound/Select.wav");		// 7
		AudioManager::instance().LoadSound("./Data/Sound/Confirm.wav");		// 8
		AudioManager::instance().LoadSound("./Data/Sound/HitBullet_1.wav");	// 9
		AudioManager::instance().LoadSound("./Data/Sound/HitBullet_2.wav");	// 10
		AudioManager::instance().LoadSound("./Data/Sound/GetItem.wav");		// 11
		AudioManager::instance().LoadSound("./Data/Sound/Charge.wav");		// 12
	}
}

void SceneBegin::Update(float elapsedTime)
{
	SceneManager::Instance().SetNextScene(std::make_shared<SceneTitle>());
}
