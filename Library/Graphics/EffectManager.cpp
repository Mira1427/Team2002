#include "EffectManager.h"

#include "Graphics.h"


// --- ������ ---
void EffectManager::initialize()
{
	Graphics& graphics = Graphics::Instance();

	// --- �����_���[���� ---
	effekseerRenderer_ = EffekseerRendererDX11::Renderer::Create(graphics.GetDevice(), graphics.GetDeviceContext(), 2048);

	// --- �}�l�[�W���[���� ---
	effekseerManager_ = Effekseer::Manager::Create(2048);

	// --- �����_���[�̊e��ݒ� ---
	effekseerManager_->SetSpriteRenderer(effekseerRenderer_->CreateSpriteRenderer());
	effekseerManager_->SetRibbonRenderer(effekseerRenderer_->CreateRibbonRenderer());
	effekseerManager_->SetRingRenderer(effekseerRenderer_->CreateRingRenderer());
	effekseerManager_->SetTrackRenderer(effekseerRenderer_->CreateTrackRenderer());
	effekseerManager_->SetModelRenderer(effekseerRenderer_->CreateModelRenderer());

	// --- ���[�_�[�̐ݒ� ---
	effekseerManager_->SetTextureLoader(effekseerRenderer_->CreateTextureLoader());
	effekseerManager_->SetModelLoader(effekseerRenderer_->CreateModelLoader());
	effekseerManager_->SetMaterialLoader(effekseerRenderer_->CreateMaterialLoader());

	// --- ������W�n�Ōv�Z ---
	effekseerManager_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}


// --- �X�V���� ---
void EffectManager::update(float elapsedTime)
{
	effekseerManager_->Update(elapsedTime * 60.0f);
}


// --- �`�揈�� ---
void EffectManager::render(const Matrix& view, const Matrix& projection)
{
	// --- �J�����̏���ݒ� ---
	effekseerRenderer_->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer_->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// --- �`�揈�� ---
	effekseerRenderer_->BeginRendering();
	effekseerManager_->Draw();
	effekseerRenderer_->EndRendering();
}
