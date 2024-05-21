#pragma once

#include <cstdint>

#include <d3d11.h>
#include <wrl.h>

class ShadowMap
{
public:
	ShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);
	virtual ~ShadowMap() = default;
	ShadowMap(const ShadowMap&) = delete;
	ShadowMap& operator=(const ShadowMap&) = delete;
	ShadowMap(const ShadowMap&&) noexcept = delete;
	ShadowMap& operator=(const ShadowMap&&) noexcept = delete;

	void Clear(ID3D11DeviceContext* dc, float depth = 1.0f);
	void Activate(ID3D11DeviceContext* dc);
	void Deactivate(ID3D11DeviceContext* dc);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView_;
	D3D11_VIEWPORT viewport_;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView_;
	UINT viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView_;
};

