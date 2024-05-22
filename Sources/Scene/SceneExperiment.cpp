#include "SceneExperiment.h"

#include <Windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <tchar.h>

#include "../../External/ImGui/imgui.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/TextureManager.h"
#include "../Graphics/DebugRenderer.h"
#include "../Graphics/ModelManager.h"

#include "../Input/InputManager.h"

#include "../GameObject/GameObject.h"

#include "../Library/Library.h"
#include "../Library/Camera.h"
#include "../Library/CameraManager.h"



// --- ������ ---
void SceneExperiment::Initialize()
{
	noise_.SetSeed(10);


	primitiveBatch_ = std::make_unique<PrimitiveBatch>(RootsLib::DX11::GetDevice(), 5000);

	geometricPrimitive_ = std::make_unique<GeometricPrimitiveBatch>(RootsLib::DX11::GetDevice(), 10001);

	model_ = std::make_shared<MeshRenderer>(RootsLib::DX11::GetDevice(), "./Data/Model/UV.fbx", true);
	model2_ = std::make_shared<MeshRenderer>(RootsLib::DX11::GetDevice(), "./Data/Model/Land.fbx", true);


	// --- �e�N�X�`���̓ǂݍ��� ---
	//TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), L"./Data/Texture/maru.png");
	//TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), L"./Data/Texture/Epic_BlueSunset_EquiRect_flat.png");

	//ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/UV.fbx", true);


	{
		auto* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj->name_ = u8"�e�X�g";

		obj->AddComponent<PrimitiveRendererComponent>();
	}


	{
		auto* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj->name_ = u8"�X�v���C�g�@�e�X�g";

		SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
		renderer->texture_ = TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), L"./Data/Texture/maru.png");
	}


	{
		obj_ = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj_->name_ = u8"PBR �e�X�g";

		MeshRendererComponent* renderer = obj_->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/stan.fbx", true, true);
		TextureManager& texManager = TextureManager::Instance();
		MeshRenderer::Material& material = renderer->model_->materials.begin()->second;
		material.shaderResourceViews[static_cast<size_t>(MaterialLabel::DIFFUSE)] = texManager.GetTexture(L"./Data/Model/stagure_color.png")->srv_;
		material.shaderResourceViews[static_cast<size_t>(MaterialLabel::NORMAL)] = texManager.GetTexture(L"./Data/Model/stagure_normal.png")->srv_;
		material.shaderResourceViews[static_cast<size_t>(MaterialLabel::ROUGHNESS)] = texManager.GetTexture(L"./Data/Model/stagure_roughness.png")->srv_;
		material.shaderResourceViews[static_cast<size_t>(MaterialLabel::METARIC)] = texManager.GetTexture(L"./Data/Model/stagure_metal.png")->srv_;

		obj_->transform_->scaling_ *= 0.005f;
	}


	{
		obj2_ = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj2_->name_ = u8"�n�`";

		obj2_->transform_->scaling_ *= 0.1f;

		MeshRendererComponent* renderer = obj2_->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/Land.fbx");
	}


	{
		playerObj_ = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		obj2_->name_ = u8"";

		obj2_->transform_->scaling_ *= 0.1f;

		MeshRendererComponent* renderer = obj2_->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/Land.fbx");
	}


	{
		pointLight_ = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		pointLight_->name_ = u8"�|�C���g���C�g";

		pointLight_->AddComponent<PointLightComponent>();
	}

	instancedMesh_ = std::make_unique<InstancedMesh>(RootsLib::DX11::GetDevice(), "./Data/Model/LandScape.fbx", true, 1000);
	instancedMesh_->materials_.begin()->second.shaderResourceViews[0] = TextureManager::Instance().GetTexture(L"./Data/Texture/Cold Sunset Equirect.png")->srv_;;

	particle_ = std::make_unique<Particle>(RootsLib::DX11::GetDevice(), 1000);
	particle_->Initialize(RootsLib::DX11::GetDeviceContext(), 0.0f);
}


// --- �I������ ---
void SceneExperiment::Finalize()
{
}


// --- �X�V���� ---
void SceneExperiment::Update(float elapsedTime)
{
	//obj2_->GetComponent<AnimatorComponent>()->Update(elapsedTime);
	pointLight_->GetComponent<PointLightComponent>()->Update();


	if (ImGui::Button(u8"������", ImVec2(200.0f, 50.0f)))
	{
		particle_->Initialize(RootsLib::DX11::GetDeviceContext(), 0.0f);
	}

	particle_->Update(RootsLib::DX11::GetDeviceContext(), elapsedTime);


	GameObjectManager::Instance().Update(elapsedTime);
	GameObjectManager::Instance().ShowDebugList();
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);
	GameObjectManager::Instance().JudgeCollision(elapsedTime);
	GameObjectManager::Instance().Remove();
}


// --- �`�揈�� ---
void SceneExperiment::Render(ID3D11DeviceContext* dc)
{
	// --- �v���~�e�B�u�`�揈�� ---
#if 0
	RootsLib::Primitive::DrawRectangle(
		dc,
		position_,
		size_,
		center_,
		angle_,
		color_
	);

	primitiveBatch_->Begin(dc);

	primitiveBatch_->Draw(
		dc,
		position_,
		size_,
		center_,
		angle_,
		color_
	);

	primitiveBatch_->End(dc);
#endif


	// --- �p�[�����m�C�Y�`��e�X�g ---
#if 0
	primitiveBatch_->Begin(dc);

	for (size_t i = 0; i < 50; i++)
	{
		for (size_t j = 0; j < 50; j++)
		{
			float color = static_cast<float>(noise_.Noise(i / 10.0f, j / 10.0f, 0));

			primitiveBatch_->Draw(
				dc,
				{ i * 10.0f, j * 10.0f },
				{ 10.0f, 10.0f },
				{ 0.0f, 0.0f },
				0.0f,
				{ color, color, color, 1.0f }
			);
		}
	}

	primitiveBatch_->End(dc);
#endif


#if 0
	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_NONE);

	if(isDrawLandScape_)
	{
		geometricPrimitive_->Begin(dc);

		for (size_t x = 0; x < 50; x++)
		{
			for (size_t z = 0; z < 50; z++)
			{
				float color = static_cast<float>(noise_.Noise(x / 10.0f, z / 10.0f, 0));
				const float height = noiseHeight_;

				Matrix T;
				Vector3 translation = modelData_.translation_;
				translation.x += static_cast<float>(x);
				translation.z += static_cast<float>(z);
				translation.y += color * height;
				T.MakeTranslation(translation);

				Vector3 scaling = modelData_.scaling_;
				scaling.y *= color * height;

				Matrix S;
				S.MakeScaling(scaling);

				Quaternion rot;
				rot.SetRotationDegFromVector(modelData_.rotation_);
				Matrix R;
				R.MakeRotationFromQuaternion(rot);

				Matrix W = S * R * T;

				geometricPrimitive_->Draw(dc, W.mat_, { color, color, color, 1.0f });
			}
		}

		geometricPrimitive_->End(dc);
	}

#endif


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

	Shader::GetShadowMap()->Deactivate(dc);

	dc->PSSetShaderResources(8, 1, Shader::GetShadowMap()->shaderResourceView_.GetAddressOf());


	// --- �J�����s��̍X�V ---
	{
		CameraComponent* cameraComp = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		data.viewProjection_ = cameraComp->viewProjection_;

		Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::SCENE), data);
	}



	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Active(dc);


	Graphics::Instance().GetSkyMapRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- �f�o�b�O���C���`�� ---
	Graphics::Instance().GetDebugLineRenderer()->Draw(dc);

	// --- �I�u�W�F�N�g�̕`�� ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(static_cast<RasterState>(rasterState_));

	instancedMesh_->Begin(dc);

	for (size_t i = 0; i < 10; i++)
	{
		for (size_t j = 0; j < 10; j++)
		{
			Matrix T;
			Vector3 translation = modelData_.translation_;
			translation.x += static_cast<float>(i * 3.0f);
			translation.z += static_cast<float>(j * 3.0f);
			T.MakeTranslation(translation);

			Vector3 scaling = modelData_.scaling_;
			scaling *= 0.001f;

			Matrix S;
			S.MakeScaling(scaling);

			Quaternion rot;
			rot.SetRotationDegFromVector(modelData_.rotation_);
			Matrix R;
			R.MakeRotationFromQuaternion(rot);

			Matrix W = S * R * T;

			instancedMesh_->Draw(dc, W, { i / 10.0f + 0.5f, i / 10.0f + 0.5f, i / 10.0f + 0.5f, 1.0f });
		}
	}

	instancedMesh_->End(dc);

	//RootsLib::Blender::SetState(BlendState::ADD);

	particle_->Draw(RootsLib::DX11::GetDeviceContext());

	//RootsLib::Blender::SetState(BlendState::NONE);


	// --- �f�o�b�O�̕`�� ---
	Graphics::Instance().GetDebugRenderer()->Draw(dc);


	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Deactive(dc);

#if 1

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


	//// --- ���ꂪ�Ȃ��Ɠ{���� ---
	//dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	//dc->VSSetShaderResources(0, 1, &nullSrv);
	//dc->PSSetShaderResources(0, 1, &nullSrv);
	//Graphics::Instance().SetRenderTarget();


	// --- ���u���[ ---
	//Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Active(dc);

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

	//Shader::GetFrameBuffer(FrameBufferLabel::HORIZONTAL_BLUR)->Deactive(dc);


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
#endif
}
