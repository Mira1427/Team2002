#include "ShadowMap.h"

#include "../Library/Misc.h"


ShadowMap::ShadowMap(ID3D11Device* device, uint32_t width, uint32_t height)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;

	D3D11_TEXTURE2D_DESC tex2dDesc = {};
	{
		tex2dDesc.Width = width;
		tex2dDesc.Height = height;
		tex2dDesc.MipLevels = 1;
		tex2dDesc.ArraySize = 1;
		tex2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		tex2dDesc.SampleDesc.Count = 1;
		tex2dDesc.SampleDesc.Quality = 0;
		tex2dDesc.Usage = D3D11_USAGE_DEFAULT;
		tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		tex2dDesc.CPUAccessFlags = 0;
		tex2dDesc.MiscFlags = 0;

		hr = device->CreateTexture2D(&tex2dDesc, 0, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	{
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;

		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	{
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceView_.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	{
		viewport_.Width = static_cast<float>(width);
		viewport_.Height = static_cast<float>(height);
		viewport_.MinDepth = 0.0f;
		viewport_.MaxDepth = 1.0f;
		viewport_.TopLeftX = 0.0f;
		viewport_.TopLeftY = 0.0f;
	}
}


// --- クリア ---
void ShadowMap::Clear(ID3D11DeviceContext* dc, float depth)
{
	// --- 深度ステンシルビューのクリア ---
	dc->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH, depth, 0U);
}


// --- 起動 ---
void ShadowMap::Activate(ID3D11DeviceContext* dc)
{
	viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	dc->RSGetViewports(&viewportCount_, cachedViewports_);
	dc->OMGetRenderTargets(1U, cachedRenderTargetView_.ReleaseAndGetAddressOf(), cachedDepthStencilView_.ReleaseAndGetAddressOf());

	dc->RSSetViewports(1U, &viewport_);
	ID3D11RenderTargetView* nullRenderTargetView = NULL;
	dc->OMSetRenderTargets(1U, &nullRenderTargetView, depthStencilView_.Get());
}


// --- 停止 ---
void ShadowMap::Deactivate(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(viewportCount_, cachedViewports_);
	dc->OMSetRenderTargets(1U, cachedRenderTargetView_.GetAddressOf(), cachedDepthStencilView_.Get());
}
