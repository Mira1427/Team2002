#include "SceneTitle.h"

#include "../GameObject/GameObject.h"

#include "../Graphics/ModelManager.h"
#include "../Graphics/Shader.h"

#include "../Input/InputManager.h"

#include "../Library/Library.h"

#include "SceneLoading.h"
#include "SceneGame.h"

#include "../../Sources/EventManager.h"


//GameObject* obj;
void SceneTitle::Initialize()
{
	EventManager::Instance().button_.state_ = ButtonState::TITLE;
	//obj = GameObjectManager::Instance().Add(
	//	std::make_shared<GameObject>()
	//);
	//obj->AddComponent<VideoComponent>();
}

void SceneTitle::Finalize()
{
}

void SceneTitle::Update(float elapsedTime)
{
	//auto* video = obj->GetComponent<VideoComponent>();
	//static bool created = false;
	//if (!created)
	//{
	//	video->Initialize("./Data/Video/demo.mp4");
	//	created = true;
	//}
	//video->Update(elapsedTime);

	EventManager::Instance().Update(elapsedTime);
	EventManager::Instance().UpdateButton();

	GameObjectManager::Instance().Update(elapsedTime);			// オブジェクトの更新
	GameObjectManager::Instance().ShowDebugList();				// デバッグリストの表示
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// デバッグGuiの表示
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// 衝突判定の処理
	GameObjectManager::Instance().Remove();						// 消去処理
}

void SceneTitle::Render(ID3D11DeviceContext* dc)
{
	// --- スカイマップの描画 ---
	Graphics::Instance().GetSkyMapRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- デバッグライン描画 ---
	//Graphics::Instance().GetDebugLineRenderer()->Draw(dc);

	// --- オブジェクトの描画 ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}
}
