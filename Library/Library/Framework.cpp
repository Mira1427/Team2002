#include "Framework.h"

#include <windows.h>

#include "../../External/ImGui/imgui.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/LightingManager.h"

#include "../Scene/SceneManager.h"
#include "../Scene/SceneExperiment.h"
#include "../Scene/SceneLoading.h"
#include "../Scene/SceneTitle.h"
#include "../Scene/SceneGame.h"

#include "../Audio/Audio.h"

#include "../Input/InputManager.h"

#include "../GameObject/GameObject.h"
#include "../GameObject/BehaviorManager.h"

#include "Library.h"
#include "Camera.h"
#include "CameraManager.h"


void Framework::Run()
{
	MSG msg = {};

	// --- ���C�����[�v ---
	while (msg.message != WM_QUIT)
	{
		// --- ���b�Z�[�W���擾 ---
		if (PeekMessage(
			&msg,		// ���b�Z�[�W�\���̂̃|�C���^
			NULL,		// �E�B���h�E�̃n���h��
			0,			// �擾���郁�b�Z�[�W�͈̔͂̍ŏ��l
			0,			// �ő�l
			PM_REMOVE	// ���b�Z�[�W�̏������@
		))
		{
			TranslateMessage(&msg);		// �L�[���̓��b�Z�[�W�̕ϊ�
			DispatchMessage(&msg);		// ���b�Z�[�W�̑��M ( �E�B���h�E�v���V�[�W���� )
		}

		// --- ���C������ ---
		else
		{
			RootsLib::Timer::Tick();					// �O�t���[���Ƃ̎��ԍ����v�Z
			RootsLib::Timer::CalculateFrameStats();		// �t���[���̓��v���̌v�Z

			Update(RootsLib::Timer::GetDeltaTime());	// �X�V����
			Render();									// �`�揈��
		}
	}
}


// --- ������ ---
bool Framework::Initialize()
{
	Graphics::Instance().InitializeRenderer();

	Graphics::Instance().InitializeManager();

	// --- ImGui�̏����� ---
	RootsLib::ImGui::Initialize();

	// --- ���͏����̏����� ---
	RootsLib::Input::Initialize();

	// --- �����V�[���̐ݒ� ---
	SceneManager::Instance().SetNextScene(std::make_shared<SceneTitle>());
	//SceneManager::Instance().SetNextScene(std::make_shared<SceneLoading>(std::make_shared<SceneGame>()));


	// --- �V�[���萔�o�b�t�@�̍쐬 ---
	Shader::CreateConstantBuffer(RootsLib::DX11::GetDevice(), Shader::GetCBAddress(ConstantBuffer::SCENE), sizeof(SceneConstant));
	Shader::CreateConstantBuffer(RootsLib::DX11::GetDevice(), Shader::GetCBAddress(ConstantBuffer::BLUR), sizeof(Vector4) * 2U);

	// --- �萔�o�b�t�@�̃o�C���h ---	
	// --- �V�[���萔 ---
	RootsLib::DX11::GetDeviceContext()->VSSetConstantBuffers(1, 1, Shader::GetCBAddress(ConstantBuffer::SCENE));
	RootsLib::DX11::GetDeviceContext()->PSSetConstantBuffers(1, 1, Shader::GetCBAddress(ConstantBuffer::SCENE));
	RootsLib::DX11::GetDeviceContext()->GSSetConstantBuffers(1, 1, Shader::GetCBAddress(ConstantBuffer::SCENE));

	// --- �u���[�萔 ---
	RootsLib::DX11::GetDeviceContext()->PSSetConstantBuffers(2, 1, Shader::GetCBAddress(ConstantBuffer::BLUR));

	// --- �萔�o�b�t�@�̍X�V ---
	// --- �K�E�X�u���[�p�̏d�݃e�[�u���̌v�Z ---
	const int NUM_WEIGHTS = 8;
	float weights[NUM_WEIGHTS];
	Shader::CalcWeightsTableFromGaussian(weights, NUM_WEIGHTS, 8.0f);
	Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::BLUR), weights);


	// --- �t���[���o�b�t�@�̍쐬 ---
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 1280, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::SCENE));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 640, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::HORIZONTAL_BLUR));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 1280, 360, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::VERTICAL_BLUR));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 1280, 720, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::LUMINANCE_EXTRACTION));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 640, 360, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR00));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 320, 180, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR01));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 160, 90, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR02));
	Shader::CreateFrameBuffer(RootsLib::DX11::GetDevice(), 80, 45, DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<size_t>(FrameBufferLabel::GAUSSIAN_BLUR03));

	// --- �|�X�g�G�t�F�N�g�p�̃s�N�Z���V�F�[�_�[�̍쐬 ---
	Shader::CreatePostEffectShaders(RootsLib::DX11::GetDevice());


	// --- �V���h�E�}�b�v�̍쐬 ---
	Shader::CreateShadowMap(RootsLib::DX11::GetDevice(), 2048, 2048);


	// --- �f�o�b�O�J�����ǉ� ---
	{
		GameObject* camera = GameObjectManager::Instance().Add(
			std::make_shared<GameObject>(),
			Vector3(),
			BehaviorManager::Instance().GetBehavior("DebugCamera")
		);

		CameraManager::Instance().currentCamera_ = camera;

		camera->name_ = u8"�f�o�b�O�J����";

		camera->transform_->rotation_.x = -20.0f;
		camera->transform_->position_.z = -10.0f;

		camera->AddComponent<CameraComponent>();
	}

	//AudioManager::instance().LoadMusic("./Data/Music/bgm.wav", 0.05f);
	//AudioManager::instance().PlayMusic(0);


	return true;
}


// --- �I������ ---
void Framework::Finalize()
{
	// --- ImGui�̏I������ ---
	RootsLib::ImGui::Finalize();
}


// --- �X�V���� ---
void Framework::Update(float elapsedTime)
{
	// --- �V�[���̕ύX���� ---
	SceneManager::Instance().ChangeScene();

	// --- ImGui�̍X�V�J�n ---
	RootsLib::ImGui::Update();

	// --- �V�X�e����Gui�X�V ---
	RootsLib::System::UpdateGui();

	// --- ���͏����̍X�V ---
	RootsLib::Input::Update();

	// --- ���y�̍X�V���� ---
	AudioManager::instance().UpdateAllVolume();

	// --- ���C�g�̍X�V���� ---
	Graphics::Instance().GetLightingManager()->Update();

	// --- �V�[���̍X�V ---
	SceneManager::Instance().Update(elapsedTime);

	// --- �J�����̍X�V ---
	Camera::Instance().Update(elapsedTime);


	// --- �V�[���萔�o�b�t�@�̍X�V ---
	{
		auto* light = Graphics::Instance().GetLightingManager();

		SceneConstant data;
		data.invGamma_ = 2.2f;
		data.gamma_ = 2.2f;
		data.windowSize_ = { RootsLib::Window::GetWidth(), RootsLib::Window::GetHeight() };


		// --- �J�����̏��� ---
		GameObject* camera = CameraManager::Instance().currentCamera_;
		CameraComponent* cameraComp = camera->GetComponent<CameraComponent>();
		data.viewProjection_ = cameraComp->viewProjection_;
		data.invViewProjection_ = cameraComp->invViewProjection_;
		Vector3 position = camera->transform_->position_;
		data.cameraPosition_ = { position.x, position.y, position.z, 1.0f };


		// --- ���C�g�̏��� ---
		data.ambientLight_.color_ = light->ambientLight_.color_;	// �A���r�G���g���C�g

		data.directionLight_.direction_ = light->directionLight_.direction_;	// �f�B���N�V�������C�g
		data.directionLight_.color_ = light->directionLight_.color_;
		data.directionLight_.intensity_ = light->directionLight_.intensity_;


		if (light->pointLights_.size() > POINT_LIGHT_MAX)
		{
			light->pointLights_.resize(POINT_LIGHT_MAX);
			OutputDebugStringW(L"�|�C���g���C�g�̐����ő吔���z���Ă��܂�\n");
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

		{
			std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());
			Shader::UpdateConstantBuffer(RootsLib::DX11::GetDeviceContext(), Shader::GetConstantBuffer(ConstantBuffer::SCENE), data);
		}
	}

	InputManager::Instance().m_.wheel_ = 0;
}


// --- �`�揈�� ---
void Framework::Render()
{
	std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

	auto* dc = RootsLib::DX11::GetDeviceContext();

	// --- �`��J�n ---
	Graphics::Instance().Begin({ 0.4f, 0.4f, 0.4f, 1.0f });


	// --- �V�[���̕`�� ---
	SceneManager::Instance().Draw(dc);


	// --- ImGui�̕`�� ---
	RootsLib::ImGui::Draw();


	// --- �`��I�� ---
	Graphics::Instance().End();
}
