#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

#include "../Math/Matrix.h"

class EffectManager
{
private:
	EffectManager() {}
	~EffectManager() {}
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;
	EffectManager(EffectManager&&) noexcept = delete;
	EffectManager& operator= (EffectManager&&) noexcept = delete;

public:
	// --- �C���X�^���X�̎擾 ---
	static EffectManager& instance()
	{
		static EffectManager instance;
		return instance;
	}

	void initialize();
	void update(float elapsedTime);
	void render(const Matrix& view, const Matrix& projection);

	Effekseer::ManagerRef getEffekseerManager() { return effekseerManager_; }

private:
	Effekseer::ManagerRef			effekseerManager_;
	EffekseerRenderer::RendererRef	effekseerRenderer_;
};

