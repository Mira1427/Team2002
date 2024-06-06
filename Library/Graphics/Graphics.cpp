#include "Graphics.h"

#include "../Library/Misc.h"


// --- 初期化 ---
void Graphics::Initialize(
	HWND hwnd,		// ウィンドウハンドル
	LONG width,		// 幅
	LONG height,	// 高さ
	BOOL fullScreen	// フルスクリーンか
)
{
	HRESULT hr = S_OK;

	// --- アダプターの生成 ---
	Microsoft::WRL::ComPtr<IDXGIFactory> factory;
	CreateDXGIFactory(IID_PPV_ARGS(&factory));
	IDXGIAdapter* adapter = {};
	for (UINT adapterIndex = 0; S_OK == factory->EnumAdapters(adapterIndex, &adapter); adapterIndex++)
	{
		DXGI_ADAPTER_DESC adapterDesc;
		adapter->GetDesc(&adapterDesc);
		if (adapterDesc.VendorId == 0x1002/*AMD*/ ||
			adapterDesc.VendorId == 0x10DE/*NVIDIA*/) 
		{
			break;
		}

		adapter->Release();
	}

	factory->Release();



	// --- デバイスとデバイスコンテキストの生成 ---
	{
		UINT createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;									// バッファの数
		swapChainDesc.BufferDesc.Width = static_cast<UINT>(width);		// 幅
		swapChainDesc.BufferDesc.Height = static_cast<UINT>(height);	// 高さ
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// フォーマット   8ビットの符号なし整数 ( 0.0 ~ 1.0 )
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;			// リフレッシュレートの分子
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			// 分母
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使用法
		swapChainDesc.OutputWindow = hwnd;								// 描画されるウィンドウのハンドル
		swapChainDesc.SampleDesc.Count = 1;								// マルチサンプリングのサンプル数
		swapChainDesc.SampleDesc.Quality = 0;							// マルチサンプリングの品質
		swapChainDesc.Windowed = !fullScreen;							// ウィンドウモードかどうか

		hr = D3D11CreateDeviceAndSwapChain(
#if 1
			adapter,							// アダプター
			D3D_DRIVER_TYPE_UNKNOWN,			// ドライバータイプの設定
#else
			NULL,								// アダプター
			D3D_DRIVER_TYPE_HARDWARE,			// ドライバータイプの設定
#endif
			NULL,								// ソフトウェアドライバータイプの設定
			createDeviceFlags,					// 作成フラグ
			&featureLevels,						// デバイスのバージョン
			1,									// 配列の要素数
			D3D11_SDK_VERSION,					// SDKのバージョン
			&swapChainDesc,						// スワップチェインの設定内容
			swapChain_.GetAddressOf(),			// スワップチェインのアドレス
			device_.GetAddressOf(),				// デバイスのアドレス
			NULL,								// デバイスの機能レベルの取得のためのポインタ
			immediateContext_.GetAddressOf()	// デバイスコンテキストのアドレス
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- レンダーターゲットビューの生成 ---
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = {};
		hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		hr = device_->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView_.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- 深度ステンシルビューの生成 ---
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer = {};
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = static_cast<UINT>(width);		// テクスチャの幅
		texture2dDesc.Height = static_cast<UINT>(height);	// 高さ
		texture2dDesc.MipLevels = 1;						// ミップマップレベルの数
		texture2dDesc.ArraySize = 1;						// テクスチャの配列の要素数
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// テクスチャのフォーマット	24ビットの深度と8ビットのステンシル
		texture2dDesc.SampleDesc.Count = 1;					// サンプリングの数
		texture2dDesc.SampleDesc.Quality = 0;				// サンプリングの品質
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;			// テクスチャの使用法
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// バインドのフラグ
		texture2dDesc.CPUAccessFlags = 0;					// CPUのアクセスフラグ
		texture2dDesc.MiscFlags = 0;						// その他のフラグ

		hr = device_->CreateTexture2D(
			&texture2dDesc,
			NULL,
			depthStencilBuffer.GetAddressOf()
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// 深度ステンシルのフォーマット	24ビットの深度と8ビットのステンシル
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// 深度ステンシルビューのビュー次元
		depthStencilViewDesc.Texture2D.MipSlice = 0;						// ミップマップレベル

		hr = device_->CreateDepthStencilView(
			depthStencilBuffer.Get(),
			&depthStencilViewDesc,
			depthStencilView_.ReleaseAndGetAddressOf()
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}


	// --- ビューポートの設定 ---
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;								// 左上のX座標
		viewport.TopLeftY = 0;								// 左上のY座標
		viewport.Width = static_cast<FLOAT>(width);			// 幅
		viewport.Height = static_cast<FLOAT>(height);		// 高さ
		viewport.MinDepth = 0.0f;							// 最低深度
		viewport.MaxDepth = 1.0f;							// 最大深度
		immediateContext_->RSSetViewports(1, &viewport);	// 設定
	}


	// --- サンプラーステートオブジェクトの生成 ---
	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[static_cast<size_t>(SamplerState::POINT)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[static_cast<size_t>(SamplerState::LINEAR)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[static_cast<size_t>(SamplerState::ANISOTROPIC)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		hr = device_->CreateSamplerState(&samplerDesc, samplerStates_[static_cast<size_t>(SamplerState::POINT_ADDRESS_BODER)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		// --- シャドウマップ用 ---
		samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDesc.BorderColor[0] = 1;
		samplerDesc.BorderColor[1] = 1;
		samplerDesc.BorderColor[2] = 1;
		samplerDesc.BorderColor[3] = 1;

		hr = device_->CreateSamplerState(
			&samplerDesc,
			samplerStates_[static_cast<size_t>(SamplerState::SHADOW)].GetAddressOf()
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- 深度ステンシルステートオブジェクトの生成 ---
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};


		//	深度テスト：オン		深度ライト：オン
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;							// Zテスト
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// Zライト
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		// テストの条件
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[1][1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	深度テスト：オフ		深度ライト：オン
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[0][1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	深度テスト：オン		深度ライト：オフ
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[1][0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	深度テスト：オフ		深度ライト：オフ
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[0][0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}


	// --- ブレンドステートオブジェクトの生成 ---
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;	// 全部無効
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device_->CreateBlendState(&blendDesc, blendStates_[static_cast<size_t>(BlendState::NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device_->CreateBlendState(&blendDesc, blendStates_[static_cast<size_t>(BlendState::ALPHA)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = TRUE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device_->CreateBlendState(&blendDesc, blendStates_[static_cast<size_t>(BlendState::ADD)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- ラスタライザステートオブジェクトの生成 ---
	{
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FrontCounterClockwise = FALSE;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.ScissorEnable = FALSE;
		rasterizerDesc.MultisampleEnable = FALSE;


		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		hr = device_->CreateRasterizerState(&rasterizerDesc, rasterStates_[static_cast<size_t>(RasterState::CULL_NONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		hr = device_->CreateRasterizerState(&rasterizerDesc, rasterStates_[static_cast<size_t>(RasterState::CULL_BACK)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_FRONT;
		hr = device_->CreateRasterizerState(&rasterizerDesc, rasterStates_[static_cast<size_t>(RasterState::CULL_FRONT)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device_->CreateRasterizerState(&rasterizerDesc, rasterStates_[static_cast<size_t>(RasterState::WIREFRAME)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_NONE;
		rasterizerDesc.AntialiasedLineEnable = TRUE;
		hr = device_->CreateRasterizerState(&rasterizerDesc, rasterStates_[static_cast<size_t>(RasterState::WIREFRAME_CULLNONE)].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}
}


// --- 描画クラスの初期化 ---
void Graphics::InitializeRenderer()
{
	primitiveRenderer_	= std::make_unique<Primitive>(device_.Get());
	spriteRenderer_		= std::make_unique<SpriteRenderer>(device_.Get());
	debugLineRenderer_	= std::make_unique<DebugLineRenderer>(device_.Get(), 100);
	debugRenderer_		= std::make_unique<DebugRenderer>(device_.Get());
	skyMapRenderer_		= std::make_unique<SkyMap>(device_.Get(), L"./Data/Texture/SkyMap/AllSky_Overcast4_Low.png");
}


// --- 管理クラスの初期化 ---
void Graphics::InitializeManager()
{
	lightingManager_ = std::make_unique<LightingManager>();
}


// --- レンダーターゲットビューとデプスステンシルビューのクリア ---
void Graphics::ClearRTVAndDSV(const Vector4& color)
{
	immediateContext_->ClearRenderTargetView(renderTargetView_.Get(), color.v_);
	immediateContext_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}


// --- レンダーターゲットの設定 ---
void Graphics::SetRenderTarget()
{
	immediateContext_->OMSetRenderTargets(
		1,
		renderTargetView_.GetAddressOf(),
		depthStencilView_.Get()
	);
}


// --- 空のレンダーターゲットビューの設定 ---
void Graphics::SetNullRTV()
{
	ID3D11RenderTargetView* nullRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediateContext_->OMSetRenderTargets(_countof(nullRenderTargetViews), nullRenderTargetViews, 0);
}


// --- 空のシェーダーリソースビューの作成 ---
void Graphics::SetNullSRV()
{
	ID3D11ShaderResourceView* nullShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediateContext_->VSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);
	immediateContext_->PSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);
}


// --- 描画の前処理 ---
void Graphics::Begin(const Vector4& color)
{
	// --- 空のレンダーターゲットビューの設定 ---
	SetNullRTV();

	// --- 空のシェーダーリソースビューの作成 ---
	SetNullSRV();

	// --- レンダーターゲットの設定 ---
	SetRenderTarget();

	// --- サンプラーステートの設定 ---
	SetSamplerState();

	// --- レンダーターゲットビューとデプスステンシルビューのクリア ---
	ClearRTVAndDSV(color);
}


// --- 描画の後処理 ---
HRESULT Graphics::End(UINT syncInterval, UINT flags)
{
	return swapChain_->Present(syncInterval, flags);
}
