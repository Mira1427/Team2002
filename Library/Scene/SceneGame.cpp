#include "SceneGame.h"

#include "../../External/ImGui/imgui.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/BehaviorManager.h"
#include "../GameObject/EraserManager.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/ModelManager.h"
#include "../Graphics/Effect.h"
#include "../Graphics/EffectManager.h"

#include "../Input/InputManager.h"

#include "../Audio/Audio.h"

#include "../Library/CameraManager.h"
#include "../Library/Library.h"

#include "../../Sources/Component/Component.h"

#include "../../Sources/EventManager.h"
#include "../../Sources/ParameterManager.h"


void SceneGame::Initialize()
{
	LightingManager* lightingManager = Graphics::Instance().GetLightingManager();
	lightingManager->directionLight_.viewSize_ = 885.0f;
	lightingManager->directionLight_.viewNearZ_ = -3000.0f;
	lightingManager->directionLight_.viewFarZ_ = 560.0f;

	EventManager::Instance().Initialize();
	EventManager::Instance().button_.state_ = ButtonState::GAME;


	// --- 弾薬ゲージの追加 ---
	auto* controller = EventManager::Instance().controller_;
	AddBulletGauge(controller->child_[0], 0);
	AddBulletGauge(controller->child_[1], 1);

	// --- スポナーの追加 ---
	AddEnemySpawner();
	ParameterManager::Instance().spawnerSmokeEffect_->play({0.0f, 10.0f, 0.0f}, Vector3::Unit_ * 5.0f, Vector3::Zero_);


	// --- 街の4等分された判定用 ---
	//AddTownLife(3, 94.0f);

	// --- ライフゲージの追加 ---
	AddLifeGauge();


	// --- 攻撃ゲージのコントローラー ---
	GameObject* attackGaugeController = AddAttackGaugeController(controller, { 80.0f, 360.0f, 0.0f });

	// --- 攻撃ゲージのバー ---
	AddAttackGaugeBar(attackGaugeController);

	// --- ウェーブ ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(160.0f, 50.0f, 0.0f),
			BehaviorManager::Instance().GetBehavior("WaveUI")
		);

		obj->name_ = u8"ウェーブのUI";
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		obj->transform_->scaling_ *= 0.8f;

		SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
		Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/wave.png");
		renderer->texture_ = texture;
		renderer->texSize_ = { 330.0f, texture->height_ * 0.1f };
	}

	// --- 色交換 ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(925.0f, 525.0f, 0.0f),
			BehaviorManager::Instance().GetBehavior("ColorItemUI")
		);

		obj->name_ = u8"色交換UI";
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");
		obj->parent_ = controller;

		obj->transform_->scaling_ *= 0.75f;

		SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
		Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/itemicon_change.png");
		renderer->texture_ = texture;
		renderer->texSize_ = { texture->width_, texture->height_ };
	}

	// --- ゲージ交換 ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(1010.0f, 600.0f, 0.0f),
			BehaviorManager::Instance().GetBehavior("GaugeItemUI")
		);

		obj->name_ = u8"攻撃交換UI";
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");
		obj->parent_ = controller;

		obj->transform_->scaling_ *= 0.75f;

		SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
		Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/itemicon_P_R.png");
		renderer->texture_ = texture;
		renderer->texSize_ = { texture->width_, texture->height_ };
	}


	AudioManager::instance().PlayMusic(1);
}


void SceneGame::Finalize()
{
	EffectManager::instance().getEffekseerManager()->StopAllEffects();
	EventManager::Instance().button_.state_ = ButtonState::OVER;

	auto* controller = EventManager::Instance().controller_;
	controller->state_--;
	controller->child_[0]->state_--;
	controller->child_[1]->state_--;

	auto* camera = CameraManager::Instance().currentCamera_;
	camera->state_--;
	camera->timer_ = 0.0f;
	auto* cameraShake = CameraManager::Instance().currentCamera_->GetComponent<CameraShakeComponent>();
	cameraShake->offset_ = Vector3::Zero_;


	AudioManager::instance().StopMusic(1);
}


void SceneGame::Update(float elapsedTime)
{
	EventManager::Instance().Update(elapsedTime);
	EventManager::Instance().UpdateButton();

#ifdef USE_IMGUI
	static Vector3 pos;
	static Vector3 size = Vector3::Unit_;
	static Vector3 rotation = Vector3::Zero_;
	ImGui::DragFloat3("pos", &pos.x);
	ImGui::DragFloat3("size", &size.x);
	ImGui::DragFloat3("rotation", &rotation.x, DirectX::XMConvertToRadians(5.0f));


	if (ImGui::Button("Effect", { 200.0f, 50.0f }))
	{
		ParameterManager::Instance().spawnerSmokeEffect_->play(pos, size, rotation.ToRadian());
	}


	if (ImGui::Button("Wave", { 200.0f, 50.0f }))
	{
	}
#endif

	if (!EventManager::Instance().paused_)
		EffectManager::instance().update(elapsedTime);

	GameObjectManager::Instance().Update(elapsedTime);			// オブジェクトの更新
	GameObjectManager::Instance().ShowDebugList();				// デバッグリストの表示
	GameObjectManager::Instance().UpdateDebugGui(elapsedTime);	// デバッグGuiの表示
	GameObjectManager::Instance().JudgeCollision(elapsedTime);	// 衝突判定の処理
	GameObjectManager::Instance().Remove();						// 消去処理
}


void SceneGame::Render(ID3D11DeviceContext* dc)
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

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- インスタンスメッシュの描画 ---
	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		ID3D11PixelShader* nullPS = NULL;
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext(), &nullPS, false);
	}

	// --- エフェクト描画 ---
	{
		CameraComponent* camera = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		EffectManager::instance().render(camera->view_, camera->projection_);
	}

	GameObjectManager::Instance().castShadow_ = true;
	GameObjectManager::Instance().Draw(dc);

	Shader::GetShadowMap()->Deactivate(dc);


	// --- シャドウマップのバインド ---
	dc->PSSetShaderResources(8, 1, Shader::GetShadowMap()->shaderResourceView_.GetAddressOf());


	// --- カメラ行列の更新 ---
	{
		CameraComponent* cameraComp = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		data.viewProjection_ = cameraComp->viewProjection_;

		Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::SCENE), data);
	}



	// --- シーンの描画 ---
	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Clear(dc);
	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Active(dc);


	// --- スカイマップの描画 ---
	Graphics::Instance().GetSkyMapRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- デバッグライン描画 ---
	//Graphics::Instance().GetDebugLineRenderer()->Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}

	// --- エフェクト描画 ---
	{
		CameraComponent* camera = CameraManager::Instance().currentCamera_->GetComponent<CameraComponent>();
		EffectManager::instance().render(camera->view_, camera->projection_);
	}

	// --- オブジェクトの描画 ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	RootsLib::Depth::SetState(DepthState::TEST_ON, DepthState::WRITE_ON);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	// --- デバッグの描画 ---
	Graphics::Instance().GetDebugRenderer()->Draw(dc);


	Shader::GetFrameBuffer(FrameBufferLabel::SCENE)->Deactive(dc);


	// --- ブルームの適用 ---
	ApplyBloom(dc);
}


// --- ブルームの適用 ---
void SceneGame::ApplyBloom(ID3D11DeviceContext* dc)
{
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
}



// --- プレイヤーの追加 ---
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

	obj->transform_->scaling_ *= 0.35f;

	// --- モデル描画コンポーネント追加 ---
	MeshRendererComponent* renderer = obj->AddComponent<MeshRendererComponent>();
	static const char* fileNames[2] = { "./Data/Model/InstancedMesh/Player/White_Train.fbx", "./Data/Model/InstancedMesh/Player/Black_Train.fbx" };
	renderer->model_ = ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), fileNames[playerNum], true, false, nullptr);
	
	// --- アニメーションコンポーネントの追加 ---
	AnimatorComponent* animator = obj->AddComponent<AnimatorComponent>();
	animator->isPlay_ = true;
	animator->isLoop_ = true;
	animator->timeScale_ = 30.0f;

	PlayerComponent* player = obj->AddComponent<PlayerComponent>();
	player->angleOffset_ = rotate;
	player->playerNum_ = playerNum;
	player->type_ = static_cast<CharacterType>(playerNum);

	return obj;
}


// --- プレイヤーのコントローラーの追加 ---
GameObject* SceneGame::AddPlayerController(float rotateSpeed, float range)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("PlayerController")
	);

	obj->name_ = u8"プレイヤーのコントローラー";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	PlayerControllerComponent* controller = obj->AddComponent<PlayerControllerComponent>();

	controller->rotateSpeed_ = rotateSpeed;	// 回転速度
	controller->range_ = range;				// 中心からの距離

	controller->maxBulletValue_ = 180.0f;	// 弾薬の最大数
	controller->addBulletValue_ = 10.0f;	// 弾薬の増加量
	controller->bulletCost_ = 10.0f;		// 弾薬のコスト

	controller->addAttackGaugeValue_ = 0.5f;	// 攻撃ゲージの増加量

	controller->maxAttackAmount_ = 5.0f;	// 最大攻撃力
	controller->minAttackAmount_ = 1.0f;	// 最小攻撃力
	controller->maxRangeAmount_ = 10.0f;	// 最大範囲
	controller->minRangeAmount_ = 1.0f;		// 最小範囲
	controller->maxAttackGaugeHeight_ = 330.0f;

	controller->laserAttackAmount_ = 5.0f;
	controller->laserSize_ = { 10.0f, 10.0f, 200.0f };

	return obj;
}


// --- ステージの追加 ---
void SceneGame::AddStage()
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>()
	);

	obj->name_ = u8"地面";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	obj->transform_->position_.y = -0.56f;
	obj->transform_->scaling_.x = 7.24f;
	obj->transform_->scaling_.z = 7.24f;

	InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Stage/ground.fbx", 5, true);
}


// --- レールの追加 ---
void SceneGame::AddRail()
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>()
	);

	obj->name_ = u8"レール";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	obj->transform_->position_.y = 0.38f;
	obj->transform_->scaling_ *= 0.3f;

	InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().GetInstancedMesh("./Data/Model/InstancedMesh/Stage/Rail.fbx", 5);
}


// --- 弾薬ゲージの追加 ---
void SceneGame::AddBulletGauge(GameObject* parent, int i)
{
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(i == 0 ? 300.0f : 980.0f,  i == 0 ? 475.0f : 255.0f, 0.0f),
			NULL
		);

		obj->name_ = u8"弾薬ゲージ背景" + std::to_string(i);
		obj->parent_ = parent;
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		obj->transform_->scaling_ *= 0.5f;
		obj->transform_->scaling_.x = 0.35f;

		if (i == 0/*左なら*/)
			obj->transform_->scaling_ *= -1.0f;


		SpriteRendererComponent* renderer = obj->AddComponent<SpriteRendererComponent>();
		Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/wakuwaku.png");
		renderer->texture_ = texture;
		renderer->texSize_ = { texture->width_, texture->height_ };
	}

	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(i == 0 ? 258.0f : 995.0f, 455.0f, 0.0f),
			BehaviorManager::Instance().GetBehavior("BulletGauge")
		);

		obj->name_ = u8"弾薬ゲージ" + std::to_string(i);
		obj->parent_ = parent;
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		obj->transform_->scaling_.x = 0.89f;

		obj->AddComponent<PrimitiveRendererComponent>();
	}
}


// --- スポナーの追加 ---
void SceneGame::AddEnemySpawner()
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("EnemySpawner")
	);

	EventManager::Instance().enemySpawner_ = obj;

	obj->name_ = u8"スポナー";
	obj->type_ = ObjectType::SPAWNER;
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	SphereCollider* collider = obj->AddCollider<SphereCollider>();
	collider->radius_ = 1.0f;

	EnemySpawnerComponent* spawner = obj->AddComponent<EnemySpawnerComponent>();
	spawner->spawnSpeed_ = 7.0f;

	//InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
	//renderer->model_ = ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Stage/Spawner.fbx", 5, true);
}


void SceneGame::AddTownLife(const int life, const float range)
{
	for (size_t i = 0; i < 4; i++)
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>()
		);

		EventManager::Instance().stages_[i] = obj;

		obj->name_ = u8"街" + std::to_string(i);
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		float angle = DirectX::XMConvertToRadians(90.0f * i);
		obj->transform_->position_.x = cosf(angle) * range;
		obj->transform_->position_.z = sinf(angle) * range;

		// --- ステージコンポーネントの追加 ---
		StageComponent* stage = obj->AddComponent<StageComponent>();
		stage->life_ = life;
	}
}


void SceneGame::AddLifeGauge()
{
	for (size_t i = 0; i < 4; i++)
	{
		GameObject* gaugeBack = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			NULL
		);
		{

			gaugeBack->state_ = static_cast<int>(i);
			gaugeBack->name_ = u8"ライフゲージの背景" + std::to_string(i);
			gaugeBack->eraser_ = EraserManager::Instance().GetEraser("Scene");

			gaugeBack->transform_->position_ = { 1120.0f, (RootsLib::Window::GetHeight() / 4.0f) * i, 0.0f };
			gaugeBack->transform_->scaling_ *= 0.695f;

			SpriteRendererComponent* renderer = gaugeBack->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/HP_base.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_, texture->height_ };
		}


		{
			GameObject* lifeGauge = GameObjectManager::Instance().Add(
				std::make_shared<GameObject>(),
				Vector3(),
				BehaviorManager::Instance().GetBehavior("LifeGauge")
			);

			lifeGauge->state_ = static_cast<int>(i);
			lifeGauge->name_ = u8"ライフゲージ" + std::to_string(i);
			lifeGauge->eraser_ = EraserManager::Instance().GetEraser("Scene");
			lifeGauge->parent_ = gaugeBack;

			lifeGauge->transform_->scaling_ *= 0.69f;

			SpriteRendererComponent* renderer = lifeGauge->AddComponent<SpriteRendererComponent>();
			Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/HP_contents.png");
			renderer->texture_ = texture;
			renderer->texSize_ = { texture->width_ / 3.0f, texture->height_ / 4.0f };
			renderer->texPos_.x = renderer->texSize_.x * 2.0f;
			renderer->texPos_.y = renderer->texSize_.y * i;
		}
	}
}


// --- 攻撃ゲージのコントローラー追加 ---
GameObject* SceneGame::AddAttackGaugeController(GameObject* parent, const Vector3& position)
{
	GameObject* attackGaugeController = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		position
	);

	attackGaugeController->name_ = u8"攻撃ゲージのコントローラー";
	attackGaugeController->eraser_ = EraserManager::Instance().GetEraser("Scene");
	attackGaugeController->parent_ = parent;

	attackGaugeController->transform_->scaling_ *= 0.67f;

	SpriteRendererComponent* renderer = attackGaugeController->AddComponent<SpriteRendererComponent>();
	Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/Gage.png");
	renderer->texture_ = texture;
	renderer->texSize_ = { texture->width_, texture->height_ };
	renderer->center_ = renderer->texSize_ * 0.5f;

	return attackGaugeController;
}


// --- 攻撃のゲージ ---
void SceneGame::AddAttackGauge(GameObject* parent, float width)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("AttackGauge")
	);

	obj->name_ = u8"攻撃ゲージ";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");
	obj->parent_ = parent;

	PrimitiveRendererComponent* renderer = obj->AddComponent<PrimitiveRendererComponent>();
	renderer->size_.x = 50.0f;
	renderer->testDepth_ = true;
	renderer->writeDepth_ = true;
	renderer->color_ = { 1.0f, 0.0f, 0.0f, 1.0f };
}


// --- 範囲ゲージの追加 ---
void SceneGame::AddRangeGauge(GameObject* parent, float width)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("RangeGauge")
	);

	obj->name_ = u8"範囲ゲージ";
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");
	obj->parent_ = parent;

	PrimitiveRendererComponent* renderer = obj->AddComponent<PrimitiveRendererComponent>();
	renderer->size_.x = 50.0f;
	renderer->testDepth_ = true;
	renderer->writeDepth_ = true;
	renderer->color_ = { 0.0f, 0.0f, 1.0f, 1.0f };
}


void SceneGame::AddAttackGaugeBar(GameObject* parent)
{
	GameObject* attackGaugeBar = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("AttackGaugeBar")
	);

	attackGaugeBar->name_ = u8"攻撃ゲージのバー";
	attackGaugeBar->parent_ = parent;
	attackGaugeBar->eraser_ = EraserManager::Instance().GetEraser("Scene");

	SpriteRendererComponent* renderer = attackGaugeBar->AddComponent<SpriteRendererComponent>();
	Texture* texture = TextureManager::Instance().GetTexture(L"./Data/Texture/UI/Gage_bar.png");
	renderer->texture_ = texture;
	renderer->texSize_ = { texture->width_, texture->height_ };
	renderer->center_ = renderer->texSize_ * 0.5f;
}
