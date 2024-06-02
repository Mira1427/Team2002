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

	GameObjectManager::Instance().Update(elapsedTime);			// �I�u�W�F�N�g�̍X�V
	GameObjectManager::Instance().ShowDebugList();				// �f�o�b�O���X�g�̕\��
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// �f�o�b�OGui�̕\��
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// �Փ˔���̏���
	GameObjectManager::Instance().Remove();						// ��������
}

void SceneTitle::Render(ID3D11DeviceContext* dc)
{
	// --- �X�J�C�}�b�v�̕`�� ---
	Graphics::Instance().GetSkyMapRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- �f�o�b�O���C���`�� ---
	//Graphics::Instance().GetDebugLineRenderer()->Draw(dc);

	// --- �I�u�W�F�N�g�̕`�� ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}
}
