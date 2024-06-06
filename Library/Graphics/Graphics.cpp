#include "Graphics.h"

#include "../Library/Misc.h"


// --- ������ ---
void Graphics::Initialize(
	HWND hwnd,		// �E�B���h�E�n���h��
	LONG width,		// ��
	LONG height,	// ����
	BOOL fullScreen	// �t���X�N���[����
)
{
	HRESULT hr = S_OK;

	// --- �A�_�v�^�[�̐��� ---
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



	// --- �f�o�C�X�ƃf�o�C�X�R���e�L�X�g�̐��� ---
	{
		UINT createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_0;

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;									// �o�b�t�@�̐�
		swapChainDesc.BufferDesc.Width = static_cast<UINT>(width);		// ��
		swapChainDesc.BufferDesc.Height = static_cast<UINT>(height);	// ����
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �t�H�[�}�b�g   8�r�b�g�̕����Ȃ����� ( 0.0 ~ 1.0 )
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;			// ���t���b�V�����[�g�̕��q
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			// ����
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �o�b�t�@�̎g�p�@
		swapChainDesc.OutputWindow = hwnd;								// �`�悳���E�B���h�E�̃n���h��
		swapChainDesc.SampleDesc.Count = 1;								// �}���`�T���v�����O�̃T���v����
		swapChainDesc.SampleDesc.Quality = 0;							// �}���`�T���v�����O�̕i��
		swapChainDesc.Windowed = !fullScreen;							// �E�B���h�E���[�h���ǂ���

		hr = D3D11CreateDeviceAndSwapChain(
#if 1
			adapter,							// �A�_�v�^�[
			D3D_DRIVER_TYPE_UNKNOWN,			// �h���C�o�[�^�C�v�̐ݒ�
#else
			NULL,								// �A�_�v�^�[
			D3D_DRIVER_TYPE_HARDWARE,			// �h���C�o�[�^�C�v�̐ݒ�
#endif
			NULL,								// �\�t�g�E�F�A�h���C�o�[�^�C�v�̐ݒ�
			createDeviceFlags,					// �쐬�t���O
			&featureLevels,						// �f�o�C�X�̃o�[�W����
			1,									// �z��̗v�f��
			D3D11_SDK_VERSION,					// SDK�̃o�[�W����
			&swapChainDesc,						// �X���b�v�`�F�C���̐ݒ���e
			swapChain_.GetAddressOf(),			// �X���b�v�`�F�C���̃A�h���X
			device_.GetAddressOf(),				// �f�o�C�X�̃A�h���X
			NULL,								// �f�o�C�X�̋@�\���x���̎擾�̂��߂̃|�C���^
			immediateContext_.GetAddressOf()	// �f�o�C�X�R���e�L�X�g�̃A�h���X
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- �����_�[�^�[�Q�b�g�r���[�̐��� ---
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer = {};
		hr = swapChain_->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		hr = device_->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView_.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- �[�x�X�e���V���r���[�̐��� ---
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer = {};
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = static_cast<UINT>(width);		// �e�N�X�`���̕�
		texture2dDesc.Height = static_cast<UINT>(height);	// ����
		texture2dDesc.MipLevels = 1;						// �~�b�v�}�b�v���x���̐�
		texture2dDesc.ArraySize = 1;						// �e�N�X�`���̔z��̗v�f��
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// �e�N�X�`���̃t�H�[�}�b�g	24�r�b�g�̐[�x��8�r�b�g�̃X�e���V��
		texture2dDesc.SampleDesc.Count = 1;					// �T���v�����O�̐�
		texture2dDesc.SampleDesc.Quality = 0;				// �T���v�����O�̕i��
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;			// �e�N�X�`���̎g�p�@
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// �o�C���h�̃t���O
		texture2dDesc.CPUAccessFlags = 0;					// CPU�̃A�N�Z�X�t���O
		texture2dDesc.MiscFlags = 0;						// ���̑��̃t���O

		hr = device_->CreateTexture2D(
			&texture2dDesc,
			NULL,
			depthStencilBuffer.GetAddressOf()
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;		// �[�x�X�e���V���̃t�H�[�}�b�g	24�r�b�g�̐[�x��8�r�b�g�̃X�e���V��
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// �[�x�X�e���V���r���[�̃r���[����
		depthStencilViewDesc.Texture2D.MipSlice = 0;						// �~�b�v�}�b�v���x��

		hr = device_->CreateDepthStencilView(
			depthStencilBuffer.Get(),
			&depthStencilViewDesc,
			depthStencilView_.ReleaseAndGetAddressOf()
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}


	// --- �r���[�|�[�g�̐ݒ� ---
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0;								// �����X���W
		viewport.TopLeftY = 0;								// �����Y���W
		viewport.Width = static_cast<FLOAT>(width);			// ��
		viewport.Height = static_cast<FLOAT>(height);		// ����
		viewport.MinDepth = 0.0f;							// �Œ�[�x
		viewport.MaxDepth = 1.0f;							// �ő�[�x
		immediateContext_->RSSetViewports(1, &viewport);	// �ݒ�
	}


	// --- �T���v���[�X�e�[�g�I�u�W�F�N�g�̐��� ---
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


		// --- �V���h�E�}�b�v�p ---
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


	// --- �[�x�X�e���V���X�e�[�g�I�u�W�F�N�g�̐��� ---
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};


		//	�[�x�e�X�g�F�I��		�[�x���C�g�F�I��
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;							// Z�e�X�g
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;	// Z���C�g
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;		// �e�X�g�̏���
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[1][1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	�[�x�e�X�g�F�I�t		�[�x���C�g�F�I��
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[0][1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	�[�x�e�X�g�F�I��		�[�x���C�g�F�I�t
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[1][0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		//	�[�x�e�X�g�F�I�t		�[�x���C�g�F�I�t
		depthStencilDesc = {};
		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device_->CreateDepthStencilState(&depthStencilDesc, depthStencilStates_[0][0].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}


	// --- �u�����h�X�e�[�g�I�u�W�F�N�g�̐��� ---
	{
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;	// �S������
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


	// --- ���X�^���C�U�X�e�[�g�I�u�W�F�N�g�̐��� ---
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


// --- �`��N���X�̏����� ---
void Graphics::InitializeRenderer()
{
	primitiveRenderer_	= std::make_unique<Primitive>(device_.Get());
	spriteRenderer_		= std::make_unique<SpriteRenderer>(device_.Get());
	debugLineRenderer_	= std::make_unique<DebugLineRenderer>(device_.Get(), 100);
	debugRenderer_		= std::make_unique<DebugRenderer>(device_.Get());
	skyMapRenderer_		= std::make_unique<SkyMap>(device_.Get(), L"./Data/Texture/SkyMap/AllSky_Overcast4_Low.png");
}


// --- �Ǘ��N���X�̏����� ---
void Graphics::InitializeManager()
{
	lightingManager_ = std::make_unique<LightingManager>();
}


// --- �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�̃N���A ---
void Graphics::ClearRTVAndDSV(const Vector4& color)
{
	immediateContext_->ClearRenderTargetView(renderTargetView_.Get(), color.v_);
	immediateContext_->ClearDepthStencilView(depthStencilView_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}


// --- �����_�[�^�[�Q�b�g�̐ݒ� ---
void Graphics::SetRenderTarget()
{
	immediateContext_->OMSetRenderTargets(
		1,
		renderTargetView_.GetAddressOf(),
		depthStencilView_.Get()
	);
}


// --- ��̃����_�[�^�[�Q�b�g�r���[�̐ݒ� ---
void Graphics::SetNullRTV()
{
	ID3D11RenderTargetView* nullRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediateContext_->OMSetRenderTargets(_countof(nullRenderTargetViews), nullRenderTargetViews, 0);
}


// --- ��̃V�F�[�_�[���\�[�X�r���[�̍쐬 ---
void Graphics::SetNullSRV()
{
	ID3D11ShaderResourceView* nullShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediateContext_->VSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);
	immediateContext_->PSSetShaderResources(0, _countof(nullShaderResourceViews), nullShaderResourceViews);
}


// --- �`��̑O���� ---
void Graphics::Begin(const Vector4& color)
{
	// --- ��̃����_�[�^�[�Q�b�g�r���[�̐ݒ� ---
	SetNullRTV();

	// --- ��̃V�F�[�_�[���\�[�X�r���[�̍쐬 ---
	SetNullSRV();

	// --- �����_�[�^�[�Q�b�g�̐ݒ� ---
	SetRenderTarget();

	// --- �T���v���[�X�e�[�g�̐ݒ� ---
	SetSamplerState();

	// --- �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�̃N���A ---
	ClearRTVAndDSV(color);
}


// --- �`��̌㏈�� ---
HRESULT Graphics::End(UINT syncInterval, UINT flags)
{
	return swapChain_->Present(syncInterval, flags);
}
