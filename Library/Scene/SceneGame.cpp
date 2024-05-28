#include "SceneGame.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/BehaviorManager.h"
#include "../GameObject/EraserManager.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ModelManager.h"

#include "../Input/InputManager.h"

#include "../Library/CameraManager.h"
#include "../Library/Library.h"

#include "../../Sources/Component/Component.h"

#include "../../Sources/EventManager.h"

void SceneGame::Initialize()
{
	LightingManager* lightingManager = Graphics::Instance().GetLightingManager();
	lightingManager->directionLight_.viewSize_ = 210.0f;
	lightingManager->directionLight_.viewNearZ_ = -90.0f;
	lightingManager->directionLight_.viewFarZ_ = 185.0f;


	// --- �J�����̐ݒ� ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("GameCamera")
		);

		obj->name_ = u8"�Q�[���J����";

		obj->AddComponent<CameraComponent>();

		CameraManager::Instance().currentCamera_ = obj;
	}


	EventManager::Instance().Initialize();
	EventManager::Instance().button_.state_ = ButtonState::GAME;


	// --- �e�ۂ̃��f���̓ǂݍ��� ---
	ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Bullet.fbx", 1000, true);

	// --- �R���g���[���[�̒ǉ� ---
	GameObject* controller = AddPlayerController(45.0f, 85.0f);

	// --- �L�����̒ǉ� ---
	GameObject* player1 = AddPlayer(u8"�L�����P", controller, 0.0f, 20.0f, 0);
	GameObject* player2 = AddPlayer(u8"�L�����Q", controller, 180.0f, 20.0f, 1);

	// --- �X�e�[�W�̒ǉ� ---
	AddStage();

	// --- �e��Q�[�W�̒ǉ� ---
	AddBulletGauge(player1, 0);
	AddBulletGauge(player2, 1);

	// --- �X�|�i�[�̒ǉ� ---
	AddEnemySpawner();

	// --- �X��4�������ꂽ����p ---
	AddTownLife(5.0f);


	// --- �U���Q�[�W�̃R���g���[���[ ---
	GameObject* attackGaugeController = AddAttackGaugeController(controller, { 1060.0f, 560.0f, 0.0f });


	// --- �U���Q�[�W ---
	AddAttackGauge(attackGaugeController, 50.0f);


	// --- �͈̓Q�[�W ---
	AddRangeGauge(attackGaugeController, 50.0f);
}


void SceneGame::Finalize()
{
}


void SceneGame::Update(float elapsedTime)
{
	EventManager::Instance().Update(elapsedTime);
	EventManager::Instance().UpdateButton();

	GameObjectManager::Instance().Update(elapsedTime);			// �I�u�W�F�N�g�̍X�V
	GameObjectManager::Instance().ShowDebugList();				// �f�o�b�O���X�g�̕\��
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// �f�o�b�OGui�̕\��
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// �Փ˔���̏���
	GameObjectManager::Instance().Remove();						// ��������
}


void SceneGame::Render(ID3D11DeviceContext* dc)
{
	SceneConstant data;

	// --- �V�[���萔�o�b�t�@�̍X�V ---
	{
		auto* light = Graphics::Instance().GetLightingManager();

		data.invGamma_ = 2.2f;
		data.gamma_ = 2.2f;
		data.windowSize_ = { RootsLib::Window::GetWidth(), RootsLib::Window::GetHeight() };

		// --- �J�����̏��� ---
		GameObject* camera = CameraManager::Instance().currentCamera_;
		CameraComponent* cameraComp = camera->GetComponent<CameraComponent>();
		data.viewProjection_ = cameraComp->viewProjection_;
		data.invViewProjection_ = cameraComp->invViewProjection_;
		Vector3 position = camera->transform_->position_;
		data.cameraPosition_ = { position.x, position.y, position.z, 1.0f };


		// --- ���C�g�̏��� ---
		data.ambientLight_.color_ = light->ambientLight_.color_;	// �A���r�G���g���C�g

		data.directionLight_.direction_ = light->directionLight_.direction_;	// �f�B���N�V�������C�g
		data.directionLight_.color_ = light->directionLight_.color_;
		data.directionLight_.intensity_ = light->directionLight_.intensity_;


		// --- �V���h�E�}�b�v�̏��� ---
		ShadowMap* shadowMap = Shader::GetShadowMap();
		LightingManager::DirectionLight& dirLight = light->directionLight_;
		const float aspectRatio = shadowMap->viewport_.Width / shadowMap->viewport_.Height;
		Matrix V;
		Vector3 eye = dirLight.viewFocus_ - Vector3::Normalize(dirLight.direction_) * dirLight.viewDistance_;
		V.MakeLookAt(eye, dirLight.viewFocus_);
		Matrix P;
		P.MakeOrthographic(dirLight.viewSize_ * aspectRatio, dirLight.viewSize_, dirLight.viewNearZ_, dirLight.viewFarZ_);
		data.viewProjection_ = V * P;
		data.directionLight_.ViewProjection_ = data.viewProjection_;


		if (light->pointLights_.size() > POINT_LIGHT_MAX)
		{
			light->pointLights_.resize(POINT_LIGHT_MAX);
			OutputDebugStringW(L"�|�C���g���C�g�̐����ő吔���z���Ă��܂�\n");
		}


		size_t index = 0;
		for (auto& point : light->pointLights_)
		{
			data.pointLights_[index].color_ = point.color_;
			data.pointLights_[index].intensity_ = point.intensity_;
			data.pointLights_[index].position_ = point.position_;
			data.pointLights_[index].range_ = point.range_;

			index++;
		}

		Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::SCENE), data);
	}


	// --- �V���h�E�}�b�v�쐬 ---
	Shader::GetShadowMap()->Clear(dc);
	Shader::GetShadowMap()->Activate(dc);

	GameObjectManager::Instance().castShadow_ = true;
	GameObjectManager::Instance().Draw(dc);

	// --- �C���X�^���X���b�V���̕`�� ---
	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		ID3D11PixelShader* nullPS = NULL;
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext(), &nullPS, false);
	}

	Shader::GetShadowMap()->Deactivate(dc);


	// --- �V���h�E�}�b�v�̃o�C���h ---
	dc->PSSetShaderResources(8, 1, Shader::GetShadowMap()->shaderResourceView_.GetAddressOf());


	// --- �J�����s��̍X�V ---
	{
		CameraComponent* cameraComp = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		data.viewProjection_ = cameraComp->viewProjection_;

		Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::SCENE), data);
	}



	// --- �V�[���̕`�� ---
	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Active(dc);


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

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- �f�o�b�O�̕`�� ---
	Graphics::Instance().GetDebugRenderer()->Draw(dc);


	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Deactive(dc);


	// --- �u���[���̓K�p ---
	ApplyBloom(dc);
}


// --- �u���[���̓K�p ---
void SceneGame::ApplyBloom(ID3D11DeviceContext* dc)
{
	// --- �P�x���o ---
	Shader::GetFrameBuffer(FrameBufferLabel::LUMINANCE_EXTRACTION)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::LUMINANCE_EXTRACTION)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::LUMINANCE_EXTRACTION),
		nullptr
	);

	Shader::GetFrameBuffer(FrameBufferLabel::LUMINANCE_EXTRACTION)->Deactive(dc);



	// --- ���u���[ ---
	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::LUMINANCE_EXTRACTION)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::HORIZONTAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Deactive(dc);


	// --- �c�u���[ ---
	Shader::GetFrameBuffer(FrameBufferLabel::VERTICAL_BLUR)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::VERTICAL_BLUR)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::VERTICAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::VERTICAL_BLUR)->Deactive(dc);


	// --- �쐣���u���[���@�P��� ---
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR00)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR00)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false
	);

	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR00)->Deactive(dc);


	// --- ���ꂪ�Ȃ��Ɠ{���� ---
	ID3D11RenderTargetView* pNullRTV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;
	dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	dc->VSSetShaderResources(0, 1, &nullSrv);
	dc->PSSetShaderResources(0, 1, &nullSrv);
	Graphics::Instance().SetRenderTarget();


	// --- ���u���[ ---
	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR00)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::HORIZONTAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Deactive(dc);


	// --- �c�u���[ ---
	// --- �쐣���u���[���@�Q��� ---	
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR01)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR01)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::VERTICAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR01)->Deactive(dc);


	// --- ���ꂪ�Ȃ��Ɠ{���� ---
	dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	dc->VSSetShaderResources(0, 1, &nullSrv);
	dc->PSSetShaderResources(0, 1, &nullSrv);
	Graphics::Instance().SetRenderTarget();


	// --- ���u���[ ---
	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR01)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::HORIZONTAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Deactive(dc);


	// --- �c�u���[ ---
	// --- �쐣���u���[���@3��� ---	
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR02)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR02)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::VERTICAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR02)->Deactive(dc);


	// --- ���ꂪ�Ȃ��Ɠ{���� ---
	dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	dc->VSSetShaderResources(0, 1, &nullSrv);
	dc->PSSetShaderResources(0, 1, &nullSrv);
	Graphics::Instance().SetRenderTarget();


	// --- ���u���[ ---
	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR02)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::HORIZONTAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Deactive(dc);


	// --- �c�u���[ ---
	// --- �쐣���u���[���@4��� ---	
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR03)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR03)->Active(dc);

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->shaderResourceViews_[0].GetAddressOf(),
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::GAUSSIAN_BLUR),
		Shader::GetVertexShader(VertexShader::VERTICAL_BLUR)
	);

	Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR03)->Deactive(dc);



	ID3D11ShaderResourceView* srvs[5] =
	{
		Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->shaderResourceViews_[0].Get(),
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR00)->shaderResourceViews_[0].Get(),
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR01)->shaderResourceViews_[0].Get(),
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR02)->shaderResourceViews_[0].Get(),
		Shader::GetFrameBuffer(FrameBufferLabel::GAUSSIAN_BLUR03)->shaderResourceViews_[0].Get(),
	};

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		srvs,
		1280.0f,
		720.0f,
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ 1280.0f, 720.0f },
		Vector2::Zero_,
		Vector3::Zero_,
		Vector4::White_,
		false,
		false,
		Shader::GetPixelShader(PixelShader::BLUR_SCENE),
		nullptr,
		0U,
		5U
	);
}



// --- �v���C���[�̒ǉ� ---
GameObject* SceneGame::AddPlayer(std::string name, GameObject* parent, float rotate, float range, int playerNum)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("BasePlayer")
	);

	obj->name_ = name;
	obj->parent_ = parent;
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	obj->transform_->scaling_ *= 0.008f;

	// --- ���f���`��R���|�[�l���g�ǉ� ---
	MeshRendererComponent* renderer = obj->AddComponent<MeshRendererComponent>();
	renderer->model_ = ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/plantune.fbx", false, false, nullptr);

	// --- �A�j���[�V�����R���|�[�l���g�̒ǉ� ---
	AnimatorComponent* animator = obj->AddComponent<AnimatorComponent>();
	animator->isPlay_ = true;
	animator->isLoop_ = true;
	animator->timeScale_ = 30.0f;

	PlayerComponent* player = obj->AddComponent<PlayerComponent>();
	player->angleOffset_ = rotate;
	player->playerNum_ = playerNum;

	return obj;
}


// --- �v���C���[�̃R���g���[���[�̒ǉ� ---
GameObject* SceneGame::AddPlayerController(float rotateSpeed, float range)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("PlayerController")
	);

	obj->name_ = u8"�v���C���[�̃R���g���[���[";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	PlayerControllerComponent* controller = obj->AddComponent<PlayerControllerComponent>();

	controller->rotateSpeed_ = rotateSpeed;	// ��]���x
	controller->range_ = range;				// ���S����̋���

	controller->maxBulletValue_ = 350.0f;	// �e��̍ő吔
	controller->addBulletValue_ = 10.0f;	// �e��̑�����
	controller->bulletCost_ = 10.0f;		// �e��̃R�X�g

	controller->addAttackGaugeValue_ = 0.5f;	// �U���Q�[�W�̑�����

	controller->maxAttackAmount_ = 5.0f;	// �ő�U����
	controller->minAttackAmount_ = 1.0f;	// �ŏ��U����
	controller->maxRangeAmount_ = 10.0f;	// �ő�͈�
	controller->minRangeAmount_ = 1.0f;		// �ŏ��͈�

	return obj;
}


// --- �X�e�[�W�̒ǉ� ---
void SceneGame::AddStage()
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>()
	);

	obj->name_ = u8"�n��";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	obj->transform_->scaling_.y = 0.02f;

	InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Stage/Stage.fbx", 5, true);
}


// --- �e��Q�[�W�̒ǉ� ---
void SceneGame::AddBulletGauge(GameObject* parent, int i)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(900.0f, 60.0f * i + 60.0f * (i + 1), 0.0f),
		BehaviorManager::Instance().GetBehavior("BulletGauge")
	);

	obj->name_ = u8"�e��Q�[�W" + std::to_string(i);
	obj->parent_ = parent;
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	PrimitiveRendererComponent* renderer = obj->AddComponent<PrimitiveRendererComponent>();
	renderer->size_.y = 60.0f;
	renderer->writeDepth_ = true;
	renderer->testDepth_ = true;
}


// --- �X�|�i�[�̒ǉ� ---
void SceneGame::AddEnemySpawner()
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("EnemySpawner")
	);

	obj->name_ = u8"�X�|�i�[";
	obj->type_ = ObjectType::SPAWNER;
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	SphereCollider* collider = obj->AddCollider<SphereCollider>();
	collider->radius_ = 1.0f;

	EnemySpawnerComponent* spawner = obj->AddComponent<EnemySpawnerComponent>();
	spawner->spawnSpeed_ = 7.0f;
}


void SceneGame::AddTownLife(const float life)
{
	for (size_t i = 0; i < 4; i++)
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		EventManager::Instance().stages_[i] = obj;

		obj->name_ = u8"�X" + std::to_string(i);
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		// --- �X�e�[�W�R���|�[�l���g�̒ǉ� ---
		StageComponent* stage = obj->AddComponent<StageComponent>();
		stage->life_ = life;
	}
}


// --- �U���Q�[�W�̃R���g���[���[�ǉ� ---
GameObject* SceneGame::AddAttackGaugeController(GameObject* parent, const Vector3& position)
{
	GameObject* attackGaugeController = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		position
	);

	attackGaugeController->name_ = u8"�U���Q�[�W�̃R���g���[���[";
	attackGaugeController->eraser_ = EraserManager::Instance().GetEraser("Scene");
	attackGaugeController->parent_ = parent;

	return attackGaugeController;
}


// --- �U���̃Q�[�W ---
void SceneGame::AddAttackGauge(GameObject* parent, float height)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("AttackGauge")
	);

	obj->name_ = u8"�U���Q�[�W";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");
	obj->parent_ = parent;

	PrimitiveRendererComponent* renderer = obj->AddComponent<PrimitiveRendererComponent>();
	renderer->size_.y = 50.0f;
	renderer->testDepth_ = true;
	renderer->writeDepth_ = true;
	renderer->color_ = { 1.0f, 0.0f, 0.0f, 1.0f };
}


// --- �͈̓Q�[�W�̒ǉ� ---
void SceneGame::AddRangeGauge(GameObject* parent, float height)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("RangeGauge")
	);

	obj->name_ = u8"�͈̓Q�[�W";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");
	obj->parent_ = parent;

	PrimitiveRendererComponent* renderer = obj->AddComponent<PrimitiveRendererComponent>();
	renderer->size_.y = 50.0f;
	renderer->testDepth_ = true;
	renderer->writeDepth_ = true;
	renderer->color_ = { 0.0f, 0.0f, 1.0f, 1.0f };
}
