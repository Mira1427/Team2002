#pragma once

#include <Effekseer.h>

#include "../Math/Matrix.h"


// ===== �G�t�F�N�g�N���X ========================================================================================================================
class Effect
{
public:
	Effect(const char* fileName);
	~Effect() {}

	// --- �Đ� ---
	Effekseer::Handle play(const Vector3& position, const Vector3& size, const Vector3& rotation);

	// --- ��~ ---
	void stop(Effekseer::Handle handle);

	// --- ���W�ݒ� ---
	void setPosition(Effekseer::Handle handle, const Vector3& position);

	// --- �X�P�[���ݒ� ---
	void setScale(Effekseer::Handle handle, const Vector3& scale);

	// --- ��]�ʐݒ� ---
	void setRotation(Effekseer::Handle handle, const Vector3& rotation);

	void SetFrame(Effekseer::Handle handle, const float frame);

private:
	Effekseer::EffectRef effekseerEffect_;
};

