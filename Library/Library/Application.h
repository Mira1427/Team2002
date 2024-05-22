#pragma once

#include <Windows.h>
#include <sstream>

#include "HighResolutionTimer.h"


class Application
{
private:
	Application() {}
	~Application() {}

public:
	static Application& Instance()
	{
		static Application instance;
		return instance;
	}


private:
	// --- ウィンドウデータの構造体 ---
	struct WindowData
	{
		LONG width_, height_;	// 幅と高さ
		BOOL isWindowed_;		// フルスクリーンか
		HWND handle_;			// ハンドル
		LPCWSTR caption_;		// クラス名
	} windowData_;


public:
	// --- ウィンドウデータの取得 ---
	WindowData& GetWindowData() { return windowData_; }

	// --- ウィンドウの初期化 ---
	HWND InitializeWindow(
		_In_ HINSTANCE instance,	// インスタンスハンドル	
		_In_ LPCWSTR caption,		// クラス名
		_In_ LONG width,			// 幅
		_In_ LONG height			// 高さ
	);	

	// --- ウィンドウプロシージャ ---
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



	// --- フレームの統計情報の計算 ---
	float CalculateFrameStats()
	{
		if (++frames_, (timer_.TimeStamp() - elapsedTime_) >= 1.0f)
		{
			float fps = static_cast<float>(frames_);
			std::wostringstream outs;
			outs.precision(6);
			outs << windowData_.caption_ << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
			SetWindowTextW(windowData_.handle_, outs.str().c_str());

			frames_ = 0;
			elapsedTime_ += 1.0f;
		}

		return elapsedTime_;
	}


	// --- 前フレームとの時間差を計算 ---
	void Tick() { timer_.Tick(); }


	// --- 経過時間の取得 ---
	float GetDeltaTime() { return timer_.TimeInterval(); }


	void UpdateSystemGui();

private:
	HighResolutionTimer timer_;
	uint32_t frames_ = 0;
	float	 elapsedTime_ = 0.0f;
};

