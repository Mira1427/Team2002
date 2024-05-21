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
	// --- �E�B���h�E�f�[�^�̍\���� ---
	struct WindowData
	{
		LONG width_, height_;	// ���ƍ���
		BOOL isWindowed_;		// �t���X�N���[����
		HWND handle_;			// �n���h��
		LPCWSTR caption_;		// �N���X��
	} windowData_;


public:
	// --- �E�B���h�E�f�[�^�̎擾 ---
	WindowData& GetWindowData() { return windowData_; }

	// --- �E�B���h�E�̏����� ---
	HWND InitializeWindow(
		_In_ HINSTANCE instance,	// �C���X�^���X�n���h��	
		_In_ LPCWSTR caption,		// �N���X��
		_In_ LONG width,			// ��
		_In_ LONG height			// ����
	);	

	// --- �E�B���h�E�v���V�[�W�� ---
	static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);



	// --- �t���[���̓��v���̌v�Z ---
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


	// --- �O�t���[���Ƃ̎��ԍ����v�Z ---
	void Tick() { timer_.Tick(); }


	// --- �o�ߎ��Ԃ̎擾 ---
	float GetDeltaTime() { return timer_.TimeInterval(); }


	void UpdateSystemGui();

private:
	HighResolutionTimer timer_;
	uint32_t frames_ = 0;
	float	 elapsedTime_ = 0.0f;
};

