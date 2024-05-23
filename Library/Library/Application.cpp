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


// --- �E�B���h�E�̏����� ---
HWND Application::InitializeWindow(
	_In_ HINSTANCE instance,	// �C���X�^���X�n���h��	
	_In_ LPCWSTR caption,		// �N���X��
	_In_ LONG width,			// ��
	_In_ LONG height			// ����
)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// ���������[�N�̌��o
	//_CrtSetBreakAlloc(####);										// �����̉񐔃��������m�ۂ��ꂽ���~
#endif


	// ---�E�B���h�E�N���X�̓o�^ ---
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);					// �\���̂̃T�C�Y��ݒ�
	wcex.style = CS_HREDRAW | CS_VREDRAW;				// �X�^�C���̐ݒ�
	wcex.lpfnWndProc = WindowProcedure;					// �E�B���h�E�̃��b�Z�[�W����������֐��̐ݒ�
	wcex.cbClsExtra = 0;								// �ǉ��̃������̐ݒ�
	wcex.cbWndExtra = 0;								//
	wcex.hInstance = instance;							// �C���X�^���X�n���h���̐ݒ�
	wcex.hIcon = 0;										// �A�C�R��
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);			// �J�[�\��
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);	// �w�i�u���V
	wcex.lpszMenuName = NULL;							// ���j���[��
	wcex.lpszClassName = caption;						// �N���X��
	wcex.hIconSm = 0;									// �������A�C�R��
	RegisterClassExW(&wcex);							// �E�B���h�E�N���X�̓o�^


	// --- �E�B���h�E�̍쐬 ---
	RECT rc{ 0, 0, width, height };						// �T�C�Y�̐ݒ�
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);	// �T�C�Y�̒���
	HWND hwnd = CreateWindowExW(
		0,														// �g���X�^�C���̐ݒ�
		caption,												// �E�B���h�E�N���X���̐ݒ� (��Őݒ肵�����O)
		L" ",													// �^�C�g����
		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME |
		WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,				// �X�^�C���̐ݒ�
		rc.right - rc.left,										// ���̐ݒ�
		rc.bottom - rc.top,										// �����̐ݒ�
		NULL,													// �e�E�B���h�E�̃n���h��
		NULL,													// �E�B���h�E���j���[�̃n���h��
		instance,												// �C���X�^���X�n���h���̐ݒ�
		NULL													// �쐬���Ɏg�p�����ǉ��̃f�[�^
	);


	// --- �E�B���h�E�̕\�� ---
	ShowWindow(hwnd, SW_SHOWDEFAULT);


	// --- �E�B���h�E�̃f�[�^�̐ݒ� ---
	windowData_.width_ = width;		// ��	
	windowData_.height_ = height;	// ����
	windowData_.handle_ = hwnd;		// �n���h��
	windowData_.caption_ = caption;	// �N���X��


	return hwnd;
}


// --- �E�B���h�E�v���V�[�W�� ---
LRESULT CALLBACK Application::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	auto& input = InputManager::instance();

	input.m_.wheelDelta_ = 0;
	input.m_.wheel_ = 0;

	// �}�E�X�z�C�[���p
	static int nWheelFraction = 0;	// ��]�ʂ̒[��

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

		// ��ʊO�Ȃ瓮�삵�Ȃ�
		if (input.getCursorPos().x < 0 || input.getCursorPos().x >=  app.windowData_.width_ ||
			input.getCursorPos().y < 0 || input.getCursorPos().y >=  app.windowData_.height_)
			break;

		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);	// ��]��

		// �O��̒[����ǉ�
		zDelta += nWheelFraction;

		// �m�b�`�������߂�
		int nNotch = zDelta / WHEEL_DELTA;

		// �[����ۑ�����
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

	ImGui::Begin(u8"�V�X�e���E�B���h�E", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar);

	static bool isOpenCamera  = false;	// �J�����̃t���O
	static bool isOpenLight   = false;	// ���C�g�̃t���O

	static bool isOpenTexture = false;	// �e�N�X�`���̃t���O
	static bool isOpenModel = false;	// ���f���̃t���O

	static bool isOpenFrameBuffer = false;	// �t���[���o�b�t�@�̃t���O
	static bool isOpenShadowMap = false;	// �V���h�E�}�b�v�̃t���O

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(u8"�V�[���؂�ւ�"))
		{
			if (ImGui::MenuItem(u8"����"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneExperiment>());
			}

			if (ImGui::MenuItem(u8"���f���G�f�B�^�["))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneModelEditor>());
			}

			if (ImGui::MenuItem(u8"�^�C�g��"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneTitle>());
			}

			if (ImGui::MenuItem(u8"�Q�[��"))
			{
				SceneManager::Instance().SetNextScene(std::make_shared<SceneGame>());
			}

			ImGui::EndMenu();
		}


		// --- �Q�[���I�u�W�F�N�g�̒ǉ� ---
		if (ImGui::BeginMenu(u8"�ǉ�"))
		{
			if (ImGui::MenuItem(u8"��̃I�u�W�F�N�g"))
			{
				GameObjectManager::Instance().Add(std::make_shared<GameObject>());
			}

			if (ImGui::MenuItem(u8"�v���~�e�B�u"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<PrimitiveRendererComponent>();
			}

			if (ImGui::MenuItem(u8"�X�v���C�g"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<SpriteRendererComponent>();
			}

			if (ImGui::MenuItem(u8"�X�P���^�����b�V��"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<MeshRendererComponent>();
			}

			if (ImGui::MenuItem(u8"�C���X�^���X���b�V��"))
			{
				GameObject* obj = GameObjectManager::Instance().Add(std::make_shared<GameObject>());
				obj->AddComponent<InstancedMeshComponent>();
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"�p�����[�^"))
		{
			if (ImGui::MenuItem(u8"�J����"))
			{
				isOpenCamera = true;
			}

			if (ImGui::MenuItem(u8"���C�g"))
			{
				isOpenLight = true;
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"�A�Z�b�g"))
		{
			// --- �e�N�X�`����Gui���J�� ---
			if (ImGui::MenuItem(u8"�e�N�X�`��"))
			{
				isOpenTexture = true;
			}

			// --- ���f����Gui���J�� ---
			if (ImGui::MenuItem(u8"���f��"))
			{
				isOpenModel = true;
			}

			// --- �X�J�C�}�b�v�̃e�N�X�`���̕ύX ---
			if (ImGui::MenuItem(u8"�X�J�C�}�b�v"))
			{
				Graphics::Instance().GetSkyMapRenderer()->OpenTexture();
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"�V�F�[�_�["))
		{
			// --- �t���[���o�b�t�@��Gui���J�� ---
			if (ImGui::MenuItem(u8"�t���[���o�b�t�@"))
			{
				isOpenFrameBuffer = true;
			}
			// --- �V���h�E�}�b�v��Gui���J�� ---
			if (ImGui::MenuItem(u8"�V���h�E�}�b�v"))
			{
				isOpenShadowMap = true;
			}

			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu(u8"�f�o�b�O"))
		{
			// --- �����蔻���\������ ---
			if (ImGui::MenuItem(u8"�R���W����"))
			{
				GameObjectManager::Instance().showCollision_ = !GameObjectManager::Instance().showCollision_;
			}

			ImGui::EndMenu();
		}


		ImGui::EndMenuBar();
	}


	ImGui::End();


	// --- �J������Gui ---
	if(isOpenCamera)
	{
		ImGui::Begin(u8"�J����", &isOpenCamera);

		Camera::Instance().UpdateDebugGui();

		ImGui::End();
	}


	// --- ���C�g��Gui ---
	if(isOpenLight)
	{
		ImGui::Begin(u8"���C�g", &isOpenLight);

		Graphics::Instance().GetLightingManager()->UpdateDebugGui();

		ImGui::End();
	}



	// --- �e�N�X�`����Gui ---
	if(isOpenTexture)
	{
		ImGui::Begin(u8"�e�N�X�`��", &isOpenTexture);

		TextureManager::Instance().UpdateDebugGui();

		ImGui::End();
	}


	// --- ���f����Gui ---
	if(isOpenModel)
	{
		ImGui::Begin(u8"���f��", &isOpenModel);

		ModelManager::Instance().UpdateDebugGui();

		ImGui::End();
	}



	// --- �t���[���o�b�t�@��Gui ---
	if(isOpenFrameBuffer)
	{
		ImGui::Begin(u8"�t���[���o�b�t�@", &isOpenFrameBuffer);

		static const char* bufferNames[static_cast<size_t>(FrameBufferLabel::MAX)] =
		{
			u8"�V�[��", u8"���u���[", u8"�c�u���[", u8"�P�x���o", u8"�쐣���u���[���P", u8"�쐣���u���[��2", u8"�쐣���u���[��3", u8"�쐣���u���[��4", 
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


	// --- �V���h�E�}�b�v��Gui ---
	if(isOpenShadowMap)
	{
		ImGui::Begin(u8"�V���h�E�}�b�v", &isOpenShadowMap);

		ImGui::Image(Shader::GetShadowMap()->shaderResourceView_.Get(), { 256.0f, 256.0f });

		ImGui::End();
	}

#endif
}
