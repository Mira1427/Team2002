#pragma once

#include <string>

#include <d3d11.h>
#include <Windows.h>

#include "../Math/Vector.h"


enum class BlendState;
enum class RasterState;
enum class DepthState;

typedef double(*EasingFunction)(double);
enum EasingFunctions;


namespace RootsLib
{
	// ライブラリの初期化 ---
	void Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height, BOOL isWindowed);


	// ===== ウィンドウ関連の処理 ======================================================================================================================================================
	namespace Window
	{
		// --- 初期化 ---
		HWND Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height);

		// --- 幅の取得 ---
		float GetWidth();

		// --- 高さの取得 ---
		float GetHeight();

		// --- ハンドルの取得 ---
		HWND GetHandle();
	}



	// ===== DirectX11関連 ======================================================================================================================================================
	namespace DX11
	{
		// --- デバイスの取得 ---
		ID3D11Device* GetDevice();

		// --- デバイスコンテキストの取得 ---
		ID3D11DeviceContext* GetDeviceContext();

		// --- スワップチェインの取得 ---
		IDXGISwapChain* GetSwapChain();
	}



	// ===== ImGui関連 ======================================================================================================================================================
	namespace ImGui
	{
		// --- 初期化 ---
		void Initialize();

		// --- 終了処理 ---
		void Finalize();

		// --- 更新処理 ---
		void Update();

		// --- 描画処理 ---
		void Draw();
	}



	// ===== タイマー関連 ======================================================================================================================================================
	namespace Timer
	{
		// --- 前フレームとの時間差を計算 ---
		void Tick();

		// --- フレームの統計情報の計算 ---
		float CalculateFrameStats();

		// --- 経過時間の取得 ---
		float GetDeltaTime();
	}



	// ===== システム関連 ======================================================================================================================================================
	namespace System
	{
		void UpdateGui();
	}



	// ===== ブレンドステート関連 ======================================================================================================================================================
	namespace Blender
	{
		void SetState(BlendState state);
	}



	// ===== ラスターステート関連 ======================================================================================================================================================
	namespace Raster
	{
		void SetState(RasterState state);
	}



	// ===== ラスターステート関連 ======================================================================================================================================================
	namespace Depth
	{
		void SetState(DepthState test, DepthState write);
	}



	// ===== プリミティブ描画 ======================================================================================================================================================
	namespace Primitive
	{
		void DrawRectangle(
			ID3D11DeviceContext* dc,
			Vector2 position,
			Vector2 size,
			Vector2 center = Vector2::Zero_,
			float angle    = 0.0f,
			Vector4 color  = Vector4::White_
		);
	}



	// ===== 入力の取得 ======================================================================================================================================================
	namespace Input
	{
		// --- 初期化 ---
		void Initialize();

		// --- 更新処理 ---
		void Update();

		// --- キーを押している間 ---
		uint64_t State(size_t padNum = 0);

		// --- キーを押した瞬間 ---
		uint64_t Down(size_t padNum = 0);

		// --- キーを離した瞬間 ---
		uint64_t Up(size_t padNum = 0);
	}



	// ===== 文字列関連 ========================================================================================================================================================================================================
	namespace String
	{
		// --- wchar_t から char への変換 ---
		std::string ConvertWideChar(const wchar_t* w);
	}



	// ===== ファイル操作関連 ========================================================================================================================================================================================================
	namespace File
	{
		// --- ファイル名の取得 ---
		std::wstring GetFileName();
	}



	// ===== 数学関連 ======================================================================================================================================================
	namespace Math
	{
		float Lerp(const float start, const float end, const float t);

		float Clampf(const float value, const float min, const float max);
		int	  Clamp(const int value, const int min, const int max);
	}



	// ===== イージング関連 ========================================================================================================================================================================================================
	namespace Easing
	{
		struct Data {

			float timer_ = 0.0f;
			float timerLimit_ = 0.0f;
			bool  isReverse_ = false;
			EasingFunction function_ = nullptr;

			Data() {};

			Data(float timerLimit, bool isReverse, EasingFunction function) :
				timerLimit_(timerLimit),
				isReverse_(isReverse),
				function_(function)
			{}

			void clear() {

				timer_ = 0.0f;
			}
		};


		EasingFunction GetFunction(EasingFunctions function);

		bool Calculate(Data& data, float elapsedTime, float& target, float increase = 1.0f, float timeScale = 1.0f);
		bool Calculate(Data* data, float elapsedTime, float& target, float increase = 1.0f, float timeScale = 1.0f);
	}
}
