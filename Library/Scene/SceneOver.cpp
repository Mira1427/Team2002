#include "SceneOver.h"

#include "../GameObject/GameObject.h"

#include "../Graphics/ModelManager.h"
#include "../Graphics/Shader.h"

#include "../Input/InputManager.h"

#include "../Library/Library.h"

#include "../../Sources/EventManager.h"

void SceneOver::Initialize()
{
}

void SceneOver::Finalize()
{
}

void SceneOver::Update(float elapsedTime)
{
	EventManager::Instance().Update(elapsedTime);

	if (InputManager::Instance().down(0) & Input::CONFIRM)
		EventManager::Instance().TranslateMessage(EventMessage::TO_TITLE_SCENE);

	GameObjectManager::Instance().Update(elapsedTime);			// オブジェクトの更新
	GameObjectManager::Instance().ShowDebugList();				// デバッグリストの表示
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// デバッグGuiの表示
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// 衝突判定の処理
	GameObjectManager::Instance().Remove();						// 消去処理
}

void SceneOver::Render(ID3D11DeviceContext* dc)
{	
	// --- スカイマップの描画 ---
	Graphics::Instance().GetSkyMapRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- オブジェクトの描画 ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}
}
