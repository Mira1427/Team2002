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



// --- 初期化 ---
void SceneExperiment::Initialize()
{
	ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/SkeletalMesh/Enemy/enemy_1walk.fbx");
	ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/SkeletalMesh/Enemy/enemy_2walk.fbx");

	ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Enemy/enemy3_gray.fbx", 100);
	ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Enemy/enemy4_white.fbx", 100);

	ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Player/dennsya2.fbx", 100);

	// --- 敵１ ---
	{
		chara1_ = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(-14.0f, 0.0f, 0.0f)
		);

		chara1_->name_ = u8"敵1";

		chara1_->transform_->scaling_ *= 0.1f;
		chara1_->transform_->rotation_.y = 180.0f;

		auto* renderer = chara1_->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/SkeletalMesh/Enemy/enemy_1walk.fbx");

		auto* animator = chara1_->AddComponent<AnimatorComponent>();
		animator->isLoop_ = animator->isPlay_ = true;
		animator->timeScale_ = 30.0f;
	}

	// --- 敵２ ---
	{
		chara2_ = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(-8.0f, 0.0f, 0.0f)
		);

		chara2_->name_ = u8"敵2";

		chara2_->transform_->scaling_ *= 0.1f;
		chara2_->transform_->rotation_.y = 180.0f;

		auto* renderer = chara2_->AddComponent<MeshRendererComponent>();
		renderer->model_ = ModelManager::Instance().GetModel("./Data/Model/SkeletalMesh/Enemy/enemy_2walk.fbx");

		auto* animator = chara2_->AddComponent<AnimatorComponent>();
		animator->isLoop_ = animator->isPlay_ = true;
		animator->timeScale_ = 30.0f;
	}

	// --- 敵３ ---
	{
		auto* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(2.5f, 0.0f, 0.0f)
		);

		obj->name_ = u8"敵3";

		obj->transform_->scaling_ *= 0.1f;
		obj->transform_->rotation_.y = 180.0f;

		auto* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Enemy/enemy3_gray.fbx", 100);
	}

	// --- 敵４ ---
	{
		auto* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(-2.5f, 0.0f, 0.0f)
		);

		obj->name_ = u8"敵4";

		obj->transform_->scaling_ *= 0.1f;
		obj->transform_->rotation_.y = 180.0f;

		auto* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Enemy/enemy4_white.fbx", 100);
	}


	// --- 自機 ---
	{
		auto* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(2.5f, 0.0f, 0.0f)
		);

		obj->name_ = u8"自機";

		obj->transform_->position_ = { -5.0f, 0.0f, 7.5f };
		obj->transform_->scaling_ *= 0.3f;
		obj->transform_->rotation_.y = 270.0f;

		auto* renderer = obj->AddComponent<InstancedMeshComponent>();
		renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Player/dennsya2.fbx", 100);
	}
}


// --- 終了処理 ---
void SceneExperiment::Finalize()
{
}


// --- 更新処理 ---
void SceneExperiment::Update(float elapsedTime)
{
	chara1_->GetComponent<AnimatorComponent>()->Update(elapsedTime);
	chara2_->GetComponent<AnimatorComponent>()->Update(elapsedTime);

	GameObjectManager::Instance().Update(elapsedTime);
	GameObjectManager::Instance().ShowDebugList();
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);
	GameObjectManager::Instance().JudgeCollision(elapsedTime);
	GameObjectManager::Instance().Remove();
}


// --- 描画処理 ---
void SceneExperiment::Render(ID3D11DeviceContext* dc)
{
	SceneConstant data;
	// --- シーン定数バッファの更新 ---
	{
		auto* light = Graphics::Instance().GetLightingManager();

		data.invGamma_ = 2.2f;
		data.gamma_ = 2.2f;
		data.windowSize_ = { RootsLib::Window::GetWidth(), RootsLib::Window::GetHeight() };

		// --- カメラの処理 ---
		GameObject* camera = CameraManager::Instance().currentCamera_;
		CameraComponent* cameraComp = camera->GetComponent<CameraComponent>();
		data.viewProjection_ = cameraComp->viewProjection_;
		data.invViewProjection_ = cameraComp->invViewProjection_;
		Vector3 position = camera->transform_->position_;
		data.cameraPosition_ = { position.x, position.y, position.z, 1.0f };


		// --- ライトの処理 ---
		data.ambientLight_.color_ = light->ambientLight_.color_;	// アンビエントライト

		data.directionLight_.direction_ = light->directionLight_.direction_;	// ディレクションライト
		data.directionLight_.color_ = light->directionLight_.color_;
		data.directionLight_.intensity_ = light->directionLight_.intensity_;


		// --- シャドウマップの処理 ---
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
			OutputDebugStringW(L"ポイントライトの数が最大数を越えています\n");
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


	// --- シャドウマップ作成 ---
	Shader::GetShadowMap()->Clear(dc);
	Shader::GetShadowMap()->Activate(dc);

	GameObjectManager::Instance().castShadow_ = true;
	GameObjectManager::Instance().Draw(dc);

	Shader::GetShadowMap()->Deactivate(dc);

	dc->PSSetShaderResources(8, 1, Shader::GetShadowMap()->shaderResourceView_.GetAddressOf());


	// --- カメラ行列の更新 ---
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

	// --- デバッグライン描画 ---
	Graphics::Instance().GetDebugLineRenderer()->Draw(dc);

	// --- オブジェクトの描画 ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}


	// --- デバッグの描画 ---
	Graphics::Instance().GetDebugRenderer()->Draw(dc);


	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Deactive(dc);

#if 1

	// --- 輝度抽出 ---
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



	// --- 横ブラー ---
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


	// --- 縦ブラー ---
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


	// --- 川瀬式ブルーム　１回目 ---
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


	// --- これがないと怒られる ---
	ID3D11RenderTargetView* pNullRTV = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;
	dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	dc->VSSetShaderResources(0, 1, &nullSrv);
	dc->PSSetShaderResources(0, 1, &nullSrv);
	Graphics::Instance().SetRenderTarget();


	// --- 横ブラー ---
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


	// --- 縦ブラー ---
	// --- 川瀬式ブルーム　２回目 ---	
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


	// --- これがないと怒られる ---
	dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	dc->VSSetShaderResources(0, 1, &nullSrv);
	dc->PSSetShaderResources(0, 1, &nullSrv);
	Graphics::Instance().SetRenderTarget();


	// --- 横ブラー ---
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


	// --- 縦ブラー ---
	// --- 川瀬式ブルーム　3回目 ---	
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


	// --- これがないと怒られる ---
	dc->OMSetRenderTargets(1, &pNullRTV, nullptr);
	dc->VSSetShaderResources(0, 1, &nullSrv);
	dc->PSSetShaderResources(0, 1, &nullSrv);
	Graphics::Instance().SetRenderTarget();


	// --- 横ブラー ---
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


	// --- 縦ブラー ---
	// --- 川瀬式ブルーム　4回目 ---	
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
