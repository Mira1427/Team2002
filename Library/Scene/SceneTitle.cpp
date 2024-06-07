#include "SceneTitle.h"

#include "../../External/ImGui/imgui.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/BehaviorManager.h"
#include "../GameObject/EraserManager.h"

#include "../Graphics/ModelManager.h"
#include "../Graphics/EffectManager.h"
#include "../Graphics/Shader.h"

#include "../Input/InputManager.h"

#include "../Library/CameraManager.h"
#include "../Library/Library.h"

#include "SceneLoading.h"
#include "SceneGame.h"

#include "../../Sources/EventManager.h"

#include "../../Sources/Component/Component.h"


void SceneTitle::Initialize()
{
	EventManager::Instance().button_.state_ = ButtonState::TITLE;

	SetUpObjects();
}

void SceneTitle::Finalize()
{
}

void SceneTitle::Update(float elapsedTime)
{
	EventManager::Instance().Update(elapsedTime);
	EventManager::Instance().UpdateButton();

	EffectManager::instance().update(elapsedTime);

	GameObjectManager::Instance().Update(elapsedTime);			// �I�u�W�F�N�g�̍X�V
	GameObjectManager::Instance().ShowDebugList();				// �f�o�b�O���X�g�̕\��
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// �f�o�b�OGui�̕\��
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// �Փ˔���̏���
	GameObjectManager::Instance().Remove();						// ��������
}

void SceneTitle::Render(ID3D11DeviceContext* dc)
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

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- �C���X�^���X���b�V���̕`�� ---
	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		ID3D11PixelShader* nullPS = NULL;
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext(), &nullPS, false);
	}

	// --- �G�t�F�N�g�`�� ---
	{
		CameraComponent* camera = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		EffectManager::instance().render(camera->view_, camera->projection_);
	}

	GameObjectManager::Instance().castShadow_ = true;
	GameObjectManager::Instance().Draw(dc);

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

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}

	// --- �G�t�F�N�g�`�� ---
	{
		CameraComponent* camera = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		EffectManager::instance().render(camera->view_, camera->projection_);
	}

	// --- �I�u�W�F�N�g�̕`�� ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- �f�o�b�O�̕`�� ---
	Graphics::Instance().GetDebugRenderer()->Draw(dc);


	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Deactive(dc);


	// --- �u���[���̓K�p ---
	ApplyBloom(dc);
}


void SceneTitle::SetUpObjects()
{
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			NULL
		);

		obj->name_ = u8"�X";
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/b.fbx", 3, true);
	}

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


	{
		GameObject* logo = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(50.0f, 50.0f, 0.0f)
		);
		
		logo->name_ = u8"�^�C�g�����S";
		logo->eraser_ = EraserManager::Instance().GetEraser("Scene");

		logo->transform_->scaling_ *= 0.7f;

		SpriteRendererComponent* renderer = logo->AddComponent<SpriteRendererComponent>();
		Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/titlelogo.png");
		renderer->texture_ = texture;
		renderer->texSize_ = { texture->width_, texture->height_ };
	}
}


void SceneTitle::ApplyBloom(ID3D11DeviceContext* dc)
{	// --- �P�x���o ---
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
