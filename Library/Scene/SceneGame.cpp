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


	// --- カメラの設定 ---
	{
		GameObject* obj = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("GameCamera")
		);

		obj->name_ = u8"ゲームカメラ";

		obj->AddComponent<CameraComponent>();

		CameraManager::Instance().currentCamera_ = obj;
	}


	EventManager::Instance().Initialize();
	EventManager::Instance().button_.state_ = ButtonState::GAME;


	// --- 弾丸のモデルの読み込み ---
	ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Bullet.fbx", 1000, true);

	// --- コントローラーの追加 ---
	GameObject* controller = AddPlayerController(45.0f, 85.0f);

	// --- キャラの追加 ---
	GameObject* player1 = AddPlayer(u8"キャラ１", controller, 0.0f, 20.0f, 0);
	GameObject* player2 = AddPlayer(u8"キャラ２", controller, 180.0f, 20.0f, 1);

	// --- ステージの追加 ---
	AddStage();

	// --- 弾薬ゲージの追加 ---
	AddBulletGauge(player1, 0);
	AddBulletGauge(player2, 1);

	// --- スポナーの追加 ---
	AddEnemySpawner();

	// --- 街の4等分された判定用 ---
	AddTownLife(5.0f);


	// --- 攻撃ゲージのコントローラー ---
	GameObject* attackGaugeController = AddAttackGaugeController(controller, { 1060.0f, 560.0f, 0.0f });


	// --- 攻撃ゲージ ---
	AddAttackGauge(attackGaugeController, 50.0f);


	// --- 範囲ゲージ ---
	AddRangeGauge(attackGaugeController, 50.0f);
}


void SceneGame::Finalize()
{
}


void SceneGame::Update(float elapsedTime)
{
	EventManager::Instance().Update(elapsedTime);
	EventManager::Instance().UpdateButton();

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

	GameObjectManager::Instance().castShadow_ = true;
	GameObjectManager::Instance().Draw(dc);

	// --- インスタンスメッシュの描画 ---
	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		ID3D11PixelShader* nullPS = NULL;
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext(), &nullPS, false);
	}

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

	// --- オブジェクトの描画 ---
	GameObjectManager::Instance().castShadow_ = false;
	GameObjectManager::Instance().Draw(dc);

	for (auto& mesh : ModelManager::Instance().GetInstancedMeshes())
	{
		mesh.second->Draw(RootsLib::DX11::GetDeviceContext());
	}

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

	obj->transform_->scaling_ *= 0.008f;

	// --- モデル描画コンポーネント追加 ---
	MeshRendererComponent* renderer = obj->AddComponent<MeshRendererComponent>();
	renderer->model_ = ModelManager::Instance().LoadModel(RootsLib::DX11::GetDevice(), "./Data/Model/plantune.fbx", false, false, nullptr);

	// --- アニメーションコンポーネントの追加 ---
	AnimatorComponent* animator = obj->AddComponent<AnimatorComponent>();
	animator->isPlay_ = true;
	animator->isLoop_ = true;
	animator->timeScale_ = 30.0f;

	PlayerComponent* player = obj->AddComponent<PlayerComponent>();
	player->angleOffset_ = rotate;
	player->playerNum_ = playerNum;

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

	controller->maxBulletValue_ = 350.0f;	// 弾薬の最大数
	controller->addBulletValue_ = 10.0f;	// 弾薬の増加量
	controller->bulletCost_ = 10.0f;		// 弾薬のコスト

	controller->addAttackGaugeValue_ = 0.5f;	// 攻撃ゲージの増加量

	controller->maxAttackAmount_ = 5.0f;	// 最大攻撃力
	controller->minAttackAmount_ = 1.0f;	// 最小攻撃力
	controller->maxRangeAmount_ = 10.0f;	// 最大範囲
	controller->minRangeAmount_ = 1.0f;		// 最小範囲

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

	obj->transform_->scaling_.y = 0.02f;

	InstancedMeshComponent* renderer = obj->AddComponent<InstancedMeshComponent>();
	renderer->model_ = ModelManager::Instance().LoadInstancedMesh(RootsLib::DX11::GetDevice(), "./Data/Model/InstancedMesh/Stage/Stage.fbx", 5, true);
}


// --- 弾薬ゲージの追加 ---
void SceneGame::AddBulletGauge(GameObject* parent, int i)
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(900.0f, 60.0f * i + 60.0f * (i + 1), 0.0f),
		BehaviorManager::Instance().GetBehavior("BulletGauge")
	);

	obj->name_ = u8"弾薬ゲージ" + std::to_string(i);
	obj->parent_ = parent;
	obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

	PrimitiveRendererComponent* renderer = obj->AddComponent<PrimitiveRendererComponent>();
	renderer->size_.y = 60.0f;
	renderer->writeDepth_ = true;
	renderer->testDepth_ = true;
}


// --- スポナーの追加 ---
void SceneGame::AddEnemySpawner()
{
	GameObject* obj = GameObjectManager::Instance().Add(
		std::make_shared<GameObject>(),
		Vector3(),
		BehaviorManager::Instance().GetBehavior("EnemySpawner")
	);

	obj->name_ = u8"スポナー";
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

		obj->name_ = u8"街" + std::to_string(i);
		obj->eraser_ = EraserManager::Instance().GetEraser("Scene");

		// --- ステージコンポーネントの追加 ---
		StageComponent* stage = obj->AddComponent<StageComponent>();
		stage->life_ = life;
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

	return attackGaugeController;
}


// --- 攻撃のゲージ ---
void SceneGame::AddAttackGauge(GameObject* parent, float height)
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
	renderer->size_.y = 50.0f;
	renderer->testDepth_ = true;
	renderer->writeDepth_ = true;
	renderer->color_ = { 1.0f, 0.0f, 0.0f, 1.0f };
}


// --- 範囲ゲージの追加 ---
void SceneGame::AddRangeGauge(GameObject* parent, float height)
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
	renderer->size_.y = 50.0f;
	renderer->testDepth_ = true;
	renderer->writeDepth_ = true;
	renderer->color_ = { 0.0f, 0.0f, 1.0f, 1.0f };
}
