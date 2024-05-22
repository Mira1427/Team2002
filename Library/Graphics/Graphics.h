#pragma once

#include <memory>
#include <mutex>

#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>


#include "../Math/Vector.h"

#include "Primitive.h"
#include "SpriteRenderer.h"
#include "DebugLineRenderer.h"
#include "LightingManager.h"
#include "DebugRenderer.h"
#include "SkyMap.h"



enum class SamplerState
{
	POINT,
	LINEAR,
	ANISOTROPIC,
	POINT_ADDRESS_BODER,
	SHADOW,
	MAX
};


enum class BlendState
{
	NONE,
	ALPHA,
	ADD,
	MAX
};


enum class RasterState
{
	CULL_NONE,
	CULL_BACK,
	CULL_FRONT,
	WIREFRAME,
	WIREFRAME_CULLNONE,
	MAX
};


enum class DepthState
{
	TEST_OFF,
	TEST_ON,
	WRITE_OFF = 0,
	WRITE_ON,
};



class Graphics
{
private:
	Graphics() {}
	~Graphics() {}

public:
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}


	ID3D11Device*			GetDevice()			{ return device_.Get(); }			// デバイスの取得
	ID3D11DeviceContext*	GetDeviceContext()	{ return immediateContext_.Get(); }	// デバイスコンテキストの取得
	IDXGISwapChain*			GetSwapChain()		{ return swapChain_.Get(); }		// スワップチェインの取得


	Primitive*			GetPrimitiveRenderer()	{ return primitiveRenderer_.get(); }	// プリミティブ描画クラス
	SpriteRenderer*		GetSpriteRenderer()		{ return spriteRenderer_.get(); }		// スプライト描画クラス
	DebugLineRenderer*	GetDebugLineRenderer()	{ return debugLineRenderer_.get(); }	// デバッグライン描画クラス
	DebugRenderer*		GetDebugRenderer()		{ return debugRenderer_.get(); }		// デバッグ描画クラス
	SkyMap*				GetSkyMapRenderer()		{ return skyMapRenderer_.get(); }		// スカイマップ描画クラス

	LightingManager* GetLightingManager() { return lightingManager_.get(); }	// ライト管理クラス


	// --- 初期化 ---
	void Initialize(
		HWND hwnd,		// ウィンドウハンドル
		LONG width,		// 幅
		LONG height,	// 高さ
		BOOL isWindowed	// フルスクリーンか
	);

	// --- 描画クラスの初期化 ---
	void InitializeRenderer();

	// --- 管理クラスの初期化 ---
	void InitializeManager();


	// --- レンダーターゲットビューとデプスステンシルビューのクリア ---
	void ClearRTVAndDSV(const Vector4& color);

	// --- レンダーターゲットの設定 ---
	void SetRenderTarget();

	// --- 空のレンダーターゲットビューの設定 ---
	void SetNullRTV();

	// --- 空のシェーダーリソースビューの作成 ---
	void SetNullSRV();


	// --- ブレンドステートの設定 ---
	void SetBlendState(BlendState state)
	{ immediateContext_->OMSetBlendState(blendStates_[static_cast<size_t>(state)].Get(), nullptr, 0xFFFFFFFF); }

	// --- サンプラーステートの設定 ---
	void SetSamplerState()
	{
		immediateContext_->PSSetSamplers(0, 1, samplerStates_[static_cast<size_t>(SamplerState::POINT)].GetAddressOf());
		immediateContext_->PSSetSamplers(1, 1, samplerStates_[static_cast<size_t>(SamplerState::LINEAR)].GetAddressOf());
		immediateContext_->PSSetSamplers(2, 1, samplerStates_[static_cast<size_t>(SamplerState::ANISOTROPIC)].GetAddressOf());
		immediateContext_->PSSetSamplers(3, 1, samplerStates_[static_cast<size_t>(SamplerState::POINT_ADDRESS_BODER)].GetAddressOf());
		immediateContext_->PSSetSamplers(4, 1, samplerStates_[static_cast<size_t>(SamplerState::SHADOW)].GetAddressOf());
	}

	// --- 深度ステンシルの設定 ---
	void SetDepthStencilState(DepthState test, DepthState write)
	{ immediateContext_->OMSetDepthStencilState(depthStencilStates_[static_cast<size_t>(test)][static_cast<size_t>(write)].Get(), 1); }

	void SetRasterState(RasterState state)
	{ immediateContext_->RSSetState(rasterStates_[static_cast<size_t>(state)].Get()); }


	// --- 描画の前処理 ---
	void Begin(const Vector4& color);

	// --- 描画の後処理 ---
	HRESULT End(UINT syncInterval = 0U, UINT flags = 0U);


	std::mutex& GetMutex() { return mutex_; }


private:
	Microsoft::WRL::ComPtr<ID3D11Device>			device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext_;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerStates_[static_cast<size_t>(SamplerState::MAX)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates_[2/*深度テスト*/][2/*深度書き込み*/];
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates_[static_cast<size_t>(BlendState::MAX)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterStates_[static_cast<size_t>(RasterState::MAX)];
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffers_[8];


	std::unique_ptr<Primitive>			primitiveRenderer_;	// プリミティブ描画クラス
	std::unique_ptr<SpriteRenderer>		spriteRenderer_;	// スプライト描画クラス
	std::unique_ptr<DebugLineRenderer>	debugLineRenderer_;	// デバッグライン描画クラス
	std::unique_ptr<LightingManager>	lightingManager_;	// ライト管理クラス
	std::unique_ptr<DebugRenderer>		debugRenderer_;		// デバッグ描画クラス
	std::unique_ptr<SkyMap>				skyMapRenderer_;	// スカイマップ描画クラス

	std::mutex mutex_;
};
