#include "Application.h"

#include <memory>
#include <assert.h>
#include <tchar.h>

#include "../ImGui/ImGuiManager.h"

#include "../Input/InputManager.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/TextureManager.h"
#include "../Graphics/ModelManager.h"
#include "../Graphics/Shader.h"

#include "Camera.h"

#include "../GameObject/GameObject.h"

#include "../Scene/SceneExperiment.h"
#include "../Scene/SceneModelEditor.h"
#include "../Scene/SceneTitle.h"
#include "../Scene/SceneGame.h"


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// --- ウィンドウの初期化 ---
HWND Application::InitializeWindow(
	_In_ HINSTANCE instance,	// インスタンスハンドル	
	_In_ LPCWSTR caption,		// クラス名
	_In_ LONG width,			// 幅
	_In_ LONG height			// 高さ
)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// メモリリークの検出
	//_CrtSetBreakAlloc(####);										// 引数の回数メモリが確保されたら停止
#endif


	// ---ウィンドウクラスの登録 ---
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);					// 構造体のサイズを設定
	wcex.style = CS_HREDRAW | CS_VREDRAW;				// スタイルの設定
	wcex.lpfnWndProc = WindowProcedure;					// ウィンドウのメッセージを処理する関数の設定
	wcex.cbClsExtra = 0;								// 追加のメモリの設定
	wcex.cbWndExtra = 0;								//
	wcex.hInstance = instance;							// インスタンスハンドルの設定
	wcex.hIcon = 0;										// アイコン
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);			// カーソル
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// 背景ブラシ
	wcex.lpszMenuName = NULL;							// メニュー名
	wcex.lpszClassName = caption;						// クラス名
	wcex.hIconSm = 0;									// 小さいアイコン
	RegisterClassExW(&wcex);							// ウィンドウクラスの登録


	// --- ウィンドウの作成 ---
	RECT rc{ 0, 0, width, height };						// サイズの設定
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);	// サイズの調整
	HWND hwnd = CreateWindowExW(
		0,														// 拡張スタイルの設定
		caption,												// ウィンドウクラス名の設定 (上で設定した名前)
		L" ",													// タイトル名
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME |
		WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,				// スタイルの設定
		rc.right - rc.left,										// 幅の設定
		rc.bottom - rc.top,										// 高さの設定
		NULL,													// 親ウィンドウのハンドル
		NULL,													// ウィンドウメニューのハンドル
		instance,												// インスタンスハンドルの設定
		NULL													// 作成時に使用される追加のデータ
	);


	// --- ウィンドウの表示 ---
	ShowWindow(hwnd, SW_SHOWDEFAULT);


	// --- ウィンドウのデータの設定 ---
	windowData_.width_ = width;		// 幅	
	windowData_.height_ = height;	// 高さ
	windowData_.handle_ = hwnd;		// ハンドル
	windowData_.caption_ = caption;	// クラス名


	return hwnd;
}


// --- ウィンドウプロシージャ ---
LRESULT CALLBACK Application::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto& input = InputManager::instance();

	input.m_.wheelDelta_ = 0;
	input.m_.wheel_ = 0;

	// マウスホイール用
	static int nWheelFraction = 0;	// 回転量の端数

#ifdef USE_IMGUI
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true;
#endif

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_ENTERSIZEMOVE:
		Application::Instance().timer_.Stop();
		break;
	case WM_EXITSIZEMOVE:
		Application::Instance().timer_.Start();
		break;

	case WM_SIZE:
	{
		//RECT clientRect = {};
		//GetClientRect(window::getHwnd(), &clientRect);
		//window::onSizeChanged(static_cast<UINT64>(clientRect.right - clientRect.left), clientRect.bottom - clientRect.top);
		break;
	}

	// --- keyboard ---
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		nWheelFraction = 0;
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {

		}
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	// --- mouse ---
	case WM_MOUSEMOVE:
		input.mouse_->ProcessMessage(msg, wParam, lParam);
		break;

	case WM_MOUSEWHEEL:
	{
		auto& input = InputManager::instance();
		auto& app = Application::Instance();

		// 画面外なら動作しない
		if (input.getCursorPos().x < 0 || input.getCursorPos().x >=  app.windowData_.width_ ||
			input.getCursorPos().y < 0 || input.getCursorPos().y >=  app.windowData_.height_)
			break;

		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// 回転量

		// 前回の端数を追加
		zDelta += nWheelFraction;

		// ノッチ数を求める
		int nNotch = zDelta / WHEEL_DELTA;

		// 端数を保存する
		nWheelFraction = zDelta % WHEEL_DELTA;

		if (nNotch == 0)
			break;

		float rate = nNotch > 0 ? 1.04f : 0.96f;

		if (nNotch > 0)
		{
			Camera::Instance().Zoom(1);
			input.m_.wheel_ = 1;
		}

		else
		{
			Camera::Instance().Zoom(-1);
			input.m_.wheel_ = -1;
		}
		break;
	}

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		InputManager::instance().mouse_->ProcessMessage(msg, wParam, lParam);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;

}



void Application::UpdateSystemGui()
{
#ifdef USE_IMGUI
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(1920.0f, 0.0f), ImGuiSetCond_Always);

	ImGui::Begin(u8"システムウィンドウ", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

	static bool isOpenCamera  = false;	// カメラのフラグ
	static bool isOpenLight   = false;	// ライトのフラグ

	static bool isOpenTexture = false;	// テクスチャのフラグ
	static bool isOpenModel = false;	// モデルのフラグ

	static bool isOpenFrameBuffer = false;	// フレームバッファのフラグ
	static bool isOpenShadowMap = false;	// シャドウマップのフラグ

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"シーン切り替え"))
		{
			if (ImGui::MenuItem(u8"実験"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneExperiment>());
			}

			if (ImGui::MenuItem(u8"モデルエディター"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneModelEditor>());
			}

			if (ImGui::MenuItem(u8"タイトル"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneTitle>());
			}

			if (ImGui::MenuItem(u8"ゲーム"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneGame>());
			}

			ImGui::EndMenu();
		}


		// --- ゲームオブジェクトの追加 ---
		if (ImGui::BeginMenu(u8"追加"))
		{
			if (ImGui::MenuItem(u8"空のオブジェクト"))
			{
				GameObjectManager::Instance().Add(std::make_shared<GameObject>());
			}

			if (ImGui::MenuItem(u8"プリミティブ"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<PrimitiveRendererComponent>();
			}

			if (ImGui::MenuItem(u8"スプライト"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<SpriteRendererComponent>();
			}

			if (ImGui::MenuItem(u8"スケルタルメッシュ"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<MeshRendererComponent>();
			}

			if (ImGui::MenuItem(u8"インスタンスメッシュ"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<InstancedMeshComponent>();
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"パラメータ"))
		{
			if (ImGui::MenuItem(u8"カメラ"))
			{
				isOpenCamera = true;
			}

			if (ImGui::MenuItem(u8"ライト"))
			{
				isOpenLight = true;
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"アセット"))
		{
			// --- テクスチャのGuiを開く ---
			if (ImGui::MenuItem(u8"テクスチャ"))
			{
				isOpenTexture = true;
			}

			// --- モデルのGuiを開く ---
			if (ImGui::MenuItem(u8"モデル"))
			{
				isOpenModel = true;
			}

			// --- スカイマップのテクスチャの変更 ---
			if (ImGui::MenuItem(u8"スカイマップ"))
			{
				Graphics::Instance().GetSkyMapRenderer()->OpenTexture();
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"シェーダー"))
		{
			// --- フレームバッファのGuiを開く ---
			if (ImGui::MenuItem(u8"フレームバッファ"))
			{
				isOpenFrameBuffer = true;
			}
			// --- シャドウマップのGuiを開く ---
			if (ImGui::MenuItem(u8"シャドウマップ"))
			{
				isOpenShadowMap = true;
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			// --- 当たり判定を表示する ---
			if (ImGui::MenuItem(u8"コリジョン"))
			{
				GameObjectManager::Instance().showCollision_ = !GameObjectManager::Instance().showCollision_;
			}

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}


	ImGui::End();


	// --- カメラのGui ---
	if(isOpenCamera)
	{
		ImGui::Begin(u8"カメラ", &isOpenCamera);

		Camera::Instance().UpdateDebugGui();

		ImGui::End();
	}


	// --- ライトのGui ---
	if(isOpenLight)
	{
		ImGui::Begin(u8"ライト", &isOpenLight);

		Graphics::Instance().GetLightingManager()->UpdateDebugGui();

		ImGui::End();
	}



	// --- テクスチャのGui ---
	if(isOpenTexture)
	{
		ImGui::Begin(u8"テクスチャ", &isOpenTexture);

		TextureManager::Instance().UpdateDebugGui();

		ImGui::End();
	}


	// --- モデルのGui ---
	if(isOpenModel)
	{
		ImGui::Begin(u8"モデル", &isOpenModel);

		ModelManager::Instance().UpdateDebugGui();

		ImGui::End();
	}



	// --- フレームバッファのGui ---
	if(isOpenFrameBuffer)
	{
		ImGui::Begin(u8"フレームバッファ", &isOpenFrameBuffer);

		static const char* bufferNames[static_cast<size_t>(FrameBufferLabel::MAX)] =
		{
			u8"シーン", u8"横ブラー", u8"縦ブラー", u8"輝度抽出", u8"川瀬式ブルーム１", u8"川瀬式ブルーム2", u8"川瀬式ブルーム3", u8"川瀬式ブルーム4", 
		};

		for (size_t i = 0; i < static_cast<size_t>(FrameBufferLabel::MAX); i++)
		{
			if (ImGui::TreeNode(bufferNames[i]))
			{
				FrameBuffer* frameBuffer = Shader::GetFrameBuffer(i);
				ImGui::Image(frameBuffer->shaderResourceViews_[0].Get(), ImVec2(frameBuffer->viewport_.Width * 0.5f, frameBuffer->viewport_.Height * 0.5f));
				ImGui::TreePop();
			}
			ImGui::Spacing();
			ImGui::Separator();
		}

		ImGui::End();
	}


	// --- シャドウマップのGui ---
	if(isOpenShadowMap)
	{
		ImGui::Begin(u8"シャドウマップ", &isOpenShadowMap);

		ImGui::Image(Shader::GetShadowMap()->shaderResourceView_.Get(), { 256.0f, 256.0f });

		ImGui::End();
	}

#endif
}
