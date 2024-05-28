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
	// --- ���C�u�����̏����� ---
	void Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height, BOOL fullScreen)
	{
		// --- �����̏����� ---
		srand(static_cast<unsigned int>(time(nullptr)));

		// --- �E�B���h�E�̏����� ---
		HWND hwnd = Window::Initialize(instance, caption, width, height);

		// --- �O���t�B�b�N�X�̏����� ---
		Graphics::Instance().Initialize(hwnd, width, height, fullScreen);
	}


	// ===== �E�B���h�E�֘A�̏��� ======================================================================================================================================================
	namespace Window
	{
		// --- ������ ---
		HWND Initialize(HINSTANCE instance, LPCWSTR caption, LONG width, LONG height)
		{
			return Application::Instance().InitializeWindow(instance, caption, width, height);
		}
		
		// --- ���̎擾 ---
		float GetWidth()
		{
			return static_cast<float>(Application::Instance().GetWindowData().width_);
		}

		// --- �����̎擾 ---
		float GetHeight()
		{
			return static_cast<float>(Application::Instance().GetWindowData().height_);
		}

		// --- �n���h���̎擾 ---
		HWND GetHandle()
		{
			return Application::Instance().GetWindowData().handle_;
		}
	}


	// ===== DirectX11�֘A ======================================================================================================================================================
	namespace DX11
	{
		// --- �f�o�C�X�̎擾 ---
		ID3D11Device* GetDevice()
		{
			return Graphics::Instance().GetDevice();
		}

		// --- �f�o�C�X�R���e�L�X�g�̎擾 ---
		ID3D11DeviceContext* GetDeviceContext()
		{
			return Graphics::Instance().GetDeviceContext();
		}

		// --- �X���b�v�`�F�C���̎擾 ---
		IDXGISwapChain* GetSwapChain()
		{
			return Graphics::Instance().GetSwapChain();
		}
	}


	// ===== ImGui�֘A ======================================================================================================================================================
	namespace ImGui
	{
		// --- ������ ---
		void Initialize()
		{
			ImGuiManager::Instance().Initialize();
		}

		// --- �I������ ---
		void Finalize()
		{
			ImGuiManager::Instance().Finalize();
		}

		// --- �X�V���� ---
		void Update()
		{
			ImGuiManager::Instance().Update();
		}

		// --- �`�揈�� ---
		void Draw()
		{
			// --- �u�����h�X�e�[�g�̐ݒ� ---
			Blender::SetState(BlendState::ALPHA);

			// --- �[�x�X�e�[�g�̐ݒ� ---
			Depth::SetState(DepthState::TEST_OFF, DepthState::WRITE_OFF);

			// --- ���X�^�[�X�e�[�g�̐ݒ� ---
			Raster::SetState(RasterState::CULL_BACK);

			ImGuiManager::Instance().Draw();
		}
	}


	// ===== �^�C�}�[�֘A ======================================================================================================================================================
	namespace Timer
	{
		// --- �O�t���[���Ƃ̎��ԍ����v�Z ---
		void Tick()
		{
			Application::Instance().Tick();
		}

		// --- �t���[���̓��v���̌v�Z ---
		float CalculateFrameStats()
		{
			return Application::Instance().CalculateFrameStats();
		}

		// --- �o�ߎ��Ԃ̎擾 ---
		float GetDeltaTime()
		{
			return Application::Instance().GetDeltaTime();
		}
	}


	// ===== �V�X�e���֘A ======================================================================================================================================================
	namespace System
	{
		void UpdateGui()
		{
			Application::Instance().UpdateSystemGui();
		}
	}


	// ===== �u�����h�X�e�[�g�֘A ======================================================================================================================================================
	void Blender::SetState(BlendState state)
	{
		Graphics::Instance().SetBlendState(state);
	}


	// ===== ���X�^�[�X�e�[�g�֘A ======================================================================================================================================================
	void Raster::SetState(RasterState state)
	{
		Graphics::Instance().SetRasterState(state);
	}


	// ===== ���X�^�[�X�e�[�g�֘A ======================================================================================================================================================
	void Depth::SetState(DepthState test, DepthState write)
	{
		Graphics::Instance().SetDepthStencilState(test, write);
	}



	// ===== �v���~�e�B�u�`�� ======================================================================================================================================================
	namespace Primitive
	{
		// --- ��`�`�� ---
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


	// ===== ���͊֘A ======================================================================================================================================================
	namespace Input
	{
		// --- ������ ---
		void Input::Initialize()
		{
			InputManager::Instance().initialize();
		}

		// --- �X�V���� ---
		void Input::Update()
		{
			InputManager::Instance().update();
		}

		// --- �L�[�������Ă���� ---
		uint64_t State(size_t padNum)
		{
			return InputManager::Instance().state(padNum);
		}

		// --- �L�[���������u�� ---
		uint64_t Down(size_t padNum)
		{
			return InputManager::Instance().down(padNum);
		}

		// --- �L�[�𗣂����u�� ---
		uint64_t Up(size_t padNum)
		{
			return InputManager::Instance().up(padNum);
		}
	}



	// ===== ������֘A ========================================================================================================================================================================================================
	namespace String
	{
		// --- wchar_t ���� char �ւ̕ϊ� ---
		std::string ConvertWideChar(const wchar_t* w)
		{
			int strlen = ::lstrlenW(w);				// ������̒������擾
			std::vector<char> c;					// ������p�̔z��錾
			c.resize(strlen * 2 + 1);				// ������ * 2���� �� �I�[���� 1�����Ƀ��T�C�Y
			::memcpy(c.data(), w, strlen * 2 + 1);	// ������̃R�s�[

			// --- �R�s�[�����������2�����ɂP�I�[�����������Ă���̂ō폜 ( ��@A '\0' B '\0' '\0' ) ---
			size_t count = 0;
			for (auto& ch : c)
			{
				// --- �k�������̍폜 ---
				if (ch == '\0')
				{
					if (count >= c.size())
						break;

					c.erase(c.begin() + count);
				}

				// --- ���������̍폜 ---
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



	// ===== �t�@�C������֘A ========================================================================================================================================================================================================
	namespace File
	{
		std::wstring GetFileName()
		{
			OPENFILENAME ofn = { 0 };
			WCHAR szFile[512] = { 0 };

			// OPENFILENAME�\���̂̃t�B�[���h��������
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL; // �E�B���h�E�̏��L�҂��w�肵�Ȃ�
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = _T("All\0*.*\0Text\0*.TXT\0"); // �t�B���^��ݒ�
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL; // �����f�B���N�g�����w�肵�Ȃ�
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR; // �p�X�ƃt�@�C�������݂��邱�Ƃ��m�F

			if (GetOpenFileName(&ofn) == TRUE)
			{
				return ofn.lpstrFile;
			}

			return L"";
		}
	}



	// ===== ���w�֘A ======================================================================================================================================================
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
