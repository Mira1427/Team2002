#include "SceneTitle.h"

#include "../../External/ImGui/imgui.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/BehaviorManager.h"
#include "../GameObject/EraserManager.h"

#include "../Graphics/ModelManager.h"
#include "../Graphics/Shader.h"

#include "../Input/InputManager.h"

#include "../Library/Library.h"

#include "SceneLoading.h"
#include "SceneGame.h"

#include "../../Sources/EventManager.h"

#include "../../Sources/Component/Component.h"


//GameObject* obj;
void SceneTitle::Initialize()
{
	//obj = GameObjectManager::Instance().Add(
	//	std::make_shared<GameObject>()
	//);
	//obj->AddComponent<VideoComponent>();

	EventManager::Instance().button_.state_ = ButtonState::TITLE;

	SetUpObjects();
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


void SceneTitle::SetUpObjects()
{
	{
		GameObject* startButton = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(800.0f, 300.0f, 0.0f),
			BehaviorManager::Instance().GetBehavior("BaseButton")
		);
		
		startButton->state_ = static_cast<int>(ButtonState::TITLE);
		startButton->name_ = u8"�X�^�[�g�{�^��";
		startButton->eraser_ = EraserManager::Instance().GetEraser("Scene");

		SpriteRendererComponent* renderer = startButton->AddComponent<SpriteRendererComponent>();
		renderer->texture_ = TextureManager::Instance().GetTexture(L"");
		renderer->texSize_ = { 300.0f, 100.0f };

		UIComponent* ui = startButton->AddComponent<UIComponent>();
		ui->eventID_ = static_cast<int>(TitleEvent::START);
	}


	{
		GameObject* startButton = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(800.0f, 450.0f, 0.0f),
			BehaviorManager::Instance().GetBehavior("BaseButton")
		);
		
		startButton->state_ = static_cast<int>(ButtonState::TITLE);
		startButton->name_ = u8"�I���{�^��";
		startButton->eraser_ = EraserManager::Instance().GetEraser("Scene");

		SpriteRendererComponent* renderer = startButton->AddComponent<SpriteRendererComponent>();
		renderer->texture_ = TextureManager::Instance().GetTexture(L"");
		renderer->texSize_ = { 300.0f, 100.0f };

		UIComponent* ui = startButton->AddComponent<UIComponent>();
		ui->eventID_ = static_cast<int>(TitleEvent::END);
	}
}
