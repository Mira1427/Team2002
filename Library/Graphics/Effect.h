#pragma once

#include <Effekseer.h>

#include "../Math/Matrix.h"


// ===== エフェクトクラス ========================================================================================================================
class Effect
{
public:
	Effect(const char* fileName);
	~Effect() {}

	// --- 再生 ---
	Effekseer::Handle play(const Vector3& position, const Vector3& size, const Vector3& rotation);

	// --- 停止 ---
	void stop(Effekseer::Handle handle);

	// --- 座標設定 ---
	void setPosition(Effekseer::Handle handle, const Vector3& position);

	// --- スケール設定 ---
	void setScale(Effekseer::Handle handle, const Vector3& scale);

	// --- 回転量設定 ---
	void setRotation(Effekseer::Handle handle, const Vector3& rotation);

	void SetFrame(Effekseer::Handle handle, const float frame);

private:
	Effekseer::EffectRef effekseerEffect_;
};

