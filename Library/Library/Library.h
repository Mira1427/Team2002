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
	// ���C�u�����̏����� ---
	void Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height, BOOL isWindowed);


	// ===== �E�B���h�E�֘A�̏��� ======================================================================================================================================================
	namespace Window
	{
		// --- ������ ---
		HWND Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height);

		// --- ���̎擾 ---
		float GetWidth();

		// --- �����̎擾 ---
		float GetHeight();

		// --- �n���h���̎擾 ---
		HWND GetHandle();
	}



	// ===== DirectX11�֘A ======================================================================================================================================================
	namespace DX11
	{
		// --- �f�o�C�X�̎擾 ---
		ID3D11Device* GetDevice();

		// --- �f�o�C�X�R���e�L�X�g�̎擾 ---
		ID3D11DeviceContext* GetDeviceContext();

		// --- �X���b�v�`�F�C���̎擾 ---
		IDXGISwapChain* GetSwapChain();
	}



	// ===== ImGui�֘A ======================================================================================================================================================
	namespace ImGui
	{
		// --- ������ ---
		void Initialize();

		// --- �I������ ---
		void Finalize();

		// --- �X�V���� ---
		void Update();

		// --- �`�揈�� ---
		void Draw();
	}



	// ===== �^�C�}�[�֘A ======================================================================================================================================================
	namespace Timer
	{
		// --- �O�t���[���Ƃ̎��ԍ����v�Z ---
		void Tick();

		// --- �t���[���̓��v���̌v�Z ---
		float CalculateFrameStats();

		// --- �o�ߎ��Ԃ̎擾 ---
		float GetDeltaTime();
	}



	// ===== �V�X�e���֘A ======================================================================================================================================================
	namespace System
	{
		void UpdateGui();
	}



	// ===== �u�����h�X�e�[�g�֘A ======================================================================================================================================================
	namespace Blender
	{
		void SetState(BlendState state);
	}



	// ===== ���X�^�[�X�e�[�g�֘A ======================================================================================================================================================
	namespace Raster
	{
		void SetState(RasterState state);
	}



	// ===== ���X�^�[�X�e�[�g�֘A ======================================================================================================================================================
	namespace Depth
	{
		void SetState(DepthState test, DepthState write);
	}



	// ===== �v���~�e�B�u�`�� ======================================================================================================================================================
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



	// ===== ���͂̎擾 ======================================================================================================================================================
	namespace Input
	{
		// --- ������ ---
		void Initialize();

		// --- �X�V���� ---
		void Update();

		// --- �L�[�������Ă���� ---
		uint64_t State(size_t padNum = 0);

		// --- �L�[���������u�� ---
		uint64_t Down(size_t padNum = 0);

		// --- �L�[�𗣂����u�� ---
		uint64_t Up(size_t padNum = 0);
	}



	// ===== ������֘A ========================================================================================================================================================================================================
	namespace String
	{
		// --- wchar_t ���� char �ւ̕ϊ� ---
		std::string ConvertWideChar(const wchar_t* w);
	}



	// ===== �t�@�C������֘A ========================================================================================================================================================================================================
	namespace File
	{
		// --- �t�@�C�����̎擾 ---
		std::wstring GetFileName();
	}



	// ===== ���w�֘A ======================================================================================================================================================
	namespace Math
	{
		float Lerp(const float start, const float end, const float t);

		float Clampf(const float value, const float min, const float max);
		int	  Clamp(const int value, const int min, const int max);
	}



	// ===== �C�[�W���O�֘A ========================================================================================================================================================================================================
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
