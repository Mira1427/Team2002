#include "Framework.h"

#include <windows.h>

#include "../../External/ImGui/imgui.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/LightingManager.h"

#include "../Scene/SceneManager.h"
#include "../Scene/SceneExperiment.h"
#include "../Scene/SceneLoading.h"

#include "../Audio/Audio.h"

#include "Library.h"
#include "Camera.h"


void Framework::Run()
{
	MSG msg = {};

	// --- メインループ ---
	while (msg.message != WM_QUIT)
	{
		// --- メッセージを取得 ---
		if (PeekMessage(
			&msg,		// メッセージ構造体のポインタ
			NULL,		// ウィンドウのハンドル
			0,			// 取得するメッセージの範囲の最小値
			0,			// 最大値
			PM_REMOVE	// メッセージの処理方法
		))
		{
			TranslateMessage(&msg);		// キー入力メッセージの変換
			DispatchMessage(&msg);		// メッセージの送信 ( ウィンドウプロシージャに )
		}

		// --- メイン処理 ---
		else
		{
			RootsLib::Timer::Tick();					// 前フレームとの時間差を計算
			RootsLib::Timer::CalculateFrameStats();		// フレームの統計情報の計算

			Update(RootsLib::Timer::GetDeltaTime());	// 更新処理
			Render();									// 描画処理
		}
	}
}


// --- 初期化 ---
bool Framework::Initialize()
{
	Graphics::Instance().InitializeRenderer();

	Graphics::Instance().InitializeManager();

	// --- ImGuiの初期化 ---
	RootsLib::ImGui::Initialize();

	// --- 入力処理の初期化 ---
	RootsLib::Input::Initialize();

	// --- 初期シーンの設定 ---
	SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneExperiment>()));


	// --- シーン定数バッファの作成 ---
	Shader::CreateConstantBuffer(RootsLib::DX11::GetDevice(), Shader::GetCBAddress(ConstantBuffer::SCENE), sizeof(SceneConstant));
	Shader::CreateConstantBuffer(RootsLib::DX11::GetDevice(), Shader::GetCBAddress(ConstantBuffer::BLUR), sizeof(Vector4) * 2U);

	// --- 定数バッファのバインド ---	
	// --- シーン定数 ---
	RootsLib::DX11::GetDeviceContext()->VSSetConstantBuffers(1, 1, Shader::GetCBAddress(ConstantBuffer::SCENE));
	RootsLib::DX11::GetDeviceContext()->PSSetConstantBuffers(1, 1, Shader::GetCBAddress(ConstantBuffer::SCENE));
	RootsLib::DX11::GetDeviceContext()->GSSetConstantBuffers(1, 1, Shader::GetCBAddress(ConstantBuffer::SCENE));

	// --- ブラー定数 ---
	RootsLib::DX11::GetDeviceContext()->PSSetConstantBuffers(2, 1, Shader::GetCBAddress(ConstantBuffer::BLUR));

	// --- 定数バッファの更新 ---
	// --- ガウスブラー用の重みテーブルの計算 ---
	const int NUM_WEIGHTS = 8;
	float weights[NUM_WEIGHTS];
	Shader::CalcWeightsTableFromGaussian(weights, NUM_WEIGHTS, 8.0f);
	Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::BLUR), weights);


	// --- フレームバッファの作成 ---
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 1280, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::SCENE));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 640, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::HORIZONTAL_BLUR));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 1280, 360, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::VERTICAL_BLUR));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 1280, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::LUMINANCE_EXTRACTION));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 640, 360, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR00));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 320, 180, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR01));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 160, 90, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR02));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 80, 45, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR03));

	// --- ポストエフェクト用のピクセルシェーダーの作成 ---
	Shader::CreatePostEffectShaders(RootsLib::DX11::GetDevice());


	// --- シャドウマップの作成 ---
	Shader::CreateShadowMap(RootsLib::DX11::GetDevice(), 2048, 2048);


	AudioManager::instance().LoadMusic("./Data/Music/bgm.wav", 0.01f);
	AudioManager::instance().PlayMusic(0);


	return true;
}


// --- 終了処理 ---
void Framework::Finalize()
{
	// --- ImGuiの終了処理 ---
	RootsLib::ImGui::Finalize();
}


// --- 更新処理 ---
void Framework::Update(float elapsedTime)
{
	// --- シーンの変更処理 ---
	SceneManager::Instance().ChangeScene();

	// --- ImGuiの更新開始 ---
	RootsLib::ImGui::Update();

	// --- システムのGui更新 ---
	RootsLib::System::UpdateGui();

	// --- 入力処理の更新 ---
	RootsLib::Input::Update();

	// --- 音楽の更新処理 ---
	AudioManager::instance().UpdateAllVolume();

	// --- ライトの更新処理 ---
	Graphics::Instance().GetLightingManager()->Update();

	// --- シーンの更新 ---
	SceneManager::Instance().Update(elapsedTime);

	// --- カメラの更新 ---
	Camera::Instance().Update(elapsedTime);


	// --- シーン定数バッファの更新 ---
	{
		auto& camera = Camera::Instance();
		auto* light = Graphics::Instance().GetLightingManager();

		SceneConstant data;
		data.invGamma_ = 2.2f;
		data.gamma_ = 2.2f;
		data.windowSize_ = { RootsLib::Window::GetWidth(), RootsLib::Window::GetHeight() };

		data.viewProjection_ = camera.GetViewProjection();
		data.invViewProjection_ = Matrix::Inverse(camera.GetViewProjection());
		Vector3 position = camera.GetPosition();
		data.cameraPosition_ = { position.x, position.y, position.z, 1.0f };


		// --- ライトの処理 ---
		data.ambientLight_.color_ = light->ambientLight_.color_;	// アンビエントライト

		data.directionLight_.direction_ = light->directionLight_.direction_;	// ディレクションライト
		data.directionLight_.color_		= light->directionLight_.color_;
		data.directionLight_.intensity_ = light->directionLight_.intensity_;


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
}


// --- 描画処理 ---
void Framework::Render()
{
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

	auto* dc = RootsLib::DX11::GetDeviceContext();

	// --- 描画開始 ---
	Graphics::Instance().Begin({ 0.4f, 0.4f, 0.4f, 1.0f });


	// --- シーンの描画 ---
	SceneManager::Instance().Draw(dc);


	// --- ImGuiの描画 ---
	RootsLib::ImGui::Draw();


	// --- 描画終了 ---
	Graphics::Instance().End();
}
