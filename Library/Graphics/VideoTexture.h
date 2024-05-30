#pragma once

#include <memory>

#include <d3d11.h>
#include <wrl.h>


// --- éQçl https://github.com/yabadabu/dx11_video_texture ---

class VideoTexture
{
public:
	static bool CreateAPI();
	static void DestroyAPI();


	bool Create(const char* fileName);
	void Destroy();
	bool Update(float elapsedTime);

	void Pause();
	void Resume();
	bool HasFinished();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture();
	float GetWidth() const;
	float GetHeight() const;
	float GetAspectRatio() const;


	class InternalData;
	InternalData* internalData_ = nullptr;
};

