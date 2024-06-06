#include "Effect.h"

#include "EffectManager.h"


// ===== �G�t�F�N�g�N���X ========================================================================================================================

// --- �R���X�g���N�^ ---
Effect::Effect(const char* fileName)
{
	// --- ���\�[�X�̓ǂݍ��� ---
	char16_t utf16FileName[256];
	Effekseer::ConvertUtf8ToUtf16(utf16FileName, 256, fileName);

	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	// --- �G�t�F�N�g�̓ǂݍ��� ---
	effekseerEffect_ = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16FileName);
}


// --- �Đ� ---
Effekseer::Handle Effect::play(const Vector3& position, const Vector3& scale, const Vector3& rotation)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect_, position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
	effekseerManager->SetRotation(handle, rotation.x, rotation.y, rotation.z);

	return handle;
}


// --- ��~ ---
void Effect::stop(Effekseer::Handle handle)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->StopEffect(handle);
}


// --- ���W�ݒ� ---
void Effect::setPosition(Effekseer::Handle handle, const Vector3& position)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}


// --- �X�P�[���ݒ� ---
void Effect::setScale(Effekseer::Handle handle, const Vector3& scale)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}


// --- ��]�ʐݒ� ---
void Effect::setRotation(Effekseer::Handle handle, const Vector3& rotation)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetRotation(handle, Effekseer::Vector3D(Vector3::AxisY_.x, Vector3::AxisY_.y, Vector3::AxisY_.z ), rotation.y);
}

void Effect::SetFrame(Effekseer::Handle handle, const float frame)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->UpdateHandleToMoveToFrame(handle, frame);
}
