#include "Library.h"

#include <time.h>
#include <vector>
#include <tchar.h>

#include "../Graphics/Graphics.h"

#include "../ImGui/ImGuiManager.h"

#include "../Input/InputManager.h"

#include "Application.h"


namespace RootsLib
{
	// --- ライブラリの初期化 ---
	void Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height, BOOL fullScreen)
	{
		// --- 乱数の初期化 ---
		srand(static_cast<unsigned int>(time(nullptr)));

		// --- ウィンドウの初期化 ---
		HWND hwnd = Window::Initialize(instance, caption, width, height);

		// --- グラフィックスの初期化 ---
		Graphics::Instance().Initialize(hwnd, width, height, fullScreen);
	}


	// ===== ウィンドウ関連の処理 ======================================================================================================================================================
	namespace Window
	{
		// --- 初期化 ---
		HWND Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height)
		{
			return Application::Instance().InitializeWindow(instance, caption, width, height);
		}
		
		// --- 幅の取得 ---
		float GetWidth()
		{
			return static_cast<float>(Application::Instance().GetWindowData().width_);
		}

		// --- 高さの取得 ---
		float GetHeight()
		{
			return static_cast<float>(Application::Instance().GetWindowData().height_);
		}

		// --- ハンドルの取得 ---
		HWND GetHandle()
		{
			return Application::Instance().GetWindowData().handle_;
		}
	}


	// ===== DirectX11関連 ======================================================================================================================================================
	namespace DX11
	{
		// --- デバイスの取得 ---
		ID3D11Device* GetDevice()
		{
			return Graphics::Instance().GetDevice();
		}

		// --- デバイスコンテキストの取得 ---
		ID3D11DeviceContext* GetDeviceContext()
		{
			return Graphics::Instance().GetDeviceContext();
		}

		// --- スワップチェインの取得 ---
		IDXGISwapChain* GetSwapChain()
		{
			return Graphics::Instance().GetSwapChain();
		}
	}


	// ===== ImGui関連 ======================================================================================================================================================
	namespace ImGui
	{
		// --- 初期化 ---
		void Initialize()
		{
			ImGuiManager::Instance().Initialize();
		}

		// --- 終了処理 ---
		void Finalize()
		{
			ImGuiManager::Instance().Finalize();
		}

		// --- 更新処理 ---
		void Update()
		{
			ImGuiManager::Instance().Update();
		}

		// --- 描画処理 ---
		void Draw()
		{
			// --- ブレンドステートの設定 ---
			Blender::SetState(BlendState::ALPHA);

			// --- 深度ステートの設定 ---
			Depth::SetState(DepthState::TEST_OFF, DepthState::WRITE_OFF);

			// --- ラスターステートの設定 ---
			Raster::SetState(RasterState::CULL_BACK);

			ImGuiManager::Instance().Draw();
		}
	}


	// ===== タイマー関連 ======================================================================================================================================================
	namespace Timer
	{
		// --- 前フレームとの時間差を計算 ---
		void Tick()
		{
			Application::Instance().Tick();
		}

		// --- フレームの統計情報の計算 ---
		float CalculateFrameStats()
		{
			return Application::Instance().CalculateFrameStats();
		}

		// --- 経過時間の取得 ---
		float GetDeltaTime()
		{
			return Application::Instance().GetDeltaTime();
		}
	}


	// ===== システム関連 ======================================================================================================================================================
	namespace System
	{
		void UpdateGui()
		{
			Application::Instance().UpdateSystemGui();
		}
	}


	// ===== ブレンドステート関連 ======================================================================================================================================================
	void Blender::SetState(BlendState state)
	{
		Graphics::Instance().SetBlendState(state);
	}


	// ===== ラスターステート関連 ======================================================================================================================================================
	void Raster::SetState(RasterState state)
	{
		Graphics::Instance().SetRasterState(state);
	}


	// ===== ラスターステート関連 ======================================================================================================================================================
	void Depth::SetState(DepthState test, DepthState write)
	{
		Graphics::Instance().SetDepthStencilState(test, write);
	}



	// ===== プリミティブ描画 ======================================================================================================================================================
	namespace Primitive
	{
		// --- 矩形描画 ---
		void DrawRectangle(
			ID3D11DeviceContext* dc,
			Vector2 position,
			Vector2 size,
			Vector2 center,
			float angle,
			Vector4 color
		)
		{
			Graphics::Instance().GetPrimitiveRenderer()->Draw(
				dc,
				position,
				size,
				center,
				angle,
				color
			);
		}
	}


	// ===== 入力関連 ======================================================================================================================================================
	namespace Input
	{
		// --- 初期化 ---
		void Input::Initialize()
		{
			InputManager::Instance().initialize();
		}

		// --- 更新処理 ---
		void Input::Update()
		{
			InputManager::Instance().update();
		}

		// --- キーを押している間 ---
		uint64_t State(size_t padNum)
		{
			return InputManager::Instance().state(padNum);
		}

		// --- キーを押した瞬間 ---
		uint64_t Down(size_t padNum)
		{
			return InputManager::Instance().down(padNum);
		}

		// --- キーを離した瞬間 ---
		uint64_t Up(size_t padNum)
		{
			return InputManager::Instance().up(padNum);
		}
	}



	// ===== 文字列関連 ========================================================================================================================================================================================================
	namespace String
	{
		// --- wchar_t から char への変換 ---
		std::string ConvertWideChar(const wchar_t* w)
		{
			int strlen = ::lstrlenW(w);				// 文字列の長さを取得
			std::vector<char> c;					// 文字列用の配列宣言
			c.resize(strlen * 2 + 1);				// 文字列 * 2文字 と 終端文字 1文字にリサイズ
			::memcpy(c.data(), w, strlen * 2 + 1);	// 文字列のコピー

			// --- コピーした文字列に2文字に１つ終端文字が入っているので削除 ( 例　A '\0' B '\0' '\0' ) ---
			size_t count = 0;
			for (auto& ch : c)
			{
				// --- ヌル文字の削除 ---
				if (ch == '\0')
				{
					if (count >= c.size())
						break;

					c.erase(c.begin() + count);
				}

				// --- 文字化けの削除 ---
				else if (ch < 0)
				{
					if (count >= c.size())
						break;

					c.erase(c.begin() + count);
				}

				count++;
			}

			std::string str = c.data();
			return str;
		}
	}



	// ===== ファイル操作関連 ========================================================================================================================================================================================================
	namespace File
	{
		std::wstring GetFileName()
		{
			OPENFILENAME ofn = { 0 };
			WCHAR szFile[512] = { 0 };

			// OPENFILENAME構造体のフィールドを初期化
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL; // ウィンドウの所有者を指定しない
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0"); // フィルタを設定
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL; // 初期ディレクトリを指定しない
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // パスとファイルが存在することを確認

			if (GetOpenFileName(&ofn) == TRUE)
			{
				return ofn.lpstrFile;
			}

			return L"";
		}
	}



	// ===== 数学関連 ======================================================================================================================================================
	namespace Math
	{
		float Lerp(const float start, const float end, const float t)
		{
			return start + (end - start) * t;
		}

		float Clampf(const float value, const float min, const float max)
		{
			return (std::max)((std::min)(value, max), min);
		}

		int Clamp(const int value, const int min, const int max)
		{
			return (std::max)((std::min)(value, max), min);
		}
	}
}
