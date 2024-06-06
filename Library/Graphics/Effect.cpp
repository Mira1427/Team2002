#include "Effect.h"

#include "EffectManager.h"


// ===== エフェクトクラス ========================================================================================================================

// --- コンストラクタ ---
Effect::Effect(const char* fileName)
{
	// --- リソースの読み込み ---
	char16_t utf16FileName[256];
	Effekseer::ConvertUtf8ToUtf16(utf16FileName, 256, fileName);

	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	// --- エフェクトの読み込み ---
	effekseerEffect_ = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16FileName);
}


// --- 再生 ---
Effekseer::Handle Effect::play(const Vector3& position, const Vector3& scale, const Vector3& rotation)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffect_, position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
	effekseerManager->SetRotation(handle, rotation.x, rotation.y, rotation.z);

	return handle;
}


// --- 停止 ---
void Effect::stop(Effekseer::Handle handle)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->StopEffect(handle);
}


// --- 座標設定 ---
void Effect::setPosition(Effekseer::Handle handle, const Vector3& position)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}


// --- スケール設定 ---
void Effect::setScale(Effekseer::Handle handle, const Vector3& scale)
{
	auto& effekseerManager = EffectManager::instance().getEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}


// --- 回転量設定 ---
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
