#include "SceneClear.h"

#include "../GameObject/GameObject.h"

#include "../Graphics/ModelManager.h"
#include "../Graphics/Shader.h"

#include "../Input/InputManager.h"

#include "../Library/Library.h"

#include "../../Sources/EventManager.h"


void SceneClear::Initialize()
{
}

void SceneClear::Finalize()
{
}

void SceneClear::Update(float elapsedTime)
{
	EventManager::Instance().Update(elapsedTime);

	GameObjectManager::Instance().Update(elapsedTime);			// �I�u�W�F�N�g�̍X�V
	GameObjectManager::Instance().ShowDebugList();				// �f�o�b�O���X�g�̕\��
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// �f�o�b�OGui�̕\��
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// �Փ˔���̏���
	GameObjectManager::Instance().Remove();						// ��������
}

void SceneClear::Render(ID3D11DeviceContext* dc)
{
	// --- �X�J�C�}�b�v�̕`�� ---
	Graphics::Instance().GetSkyMapRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- �I�u�W�F�N�g�̕`�� ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}

}
