#include "Shader.h"

#include <filesystem>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>


// --- �萔�o�b�t�@ ---
Microsoft::WRL::ComPtr<ID3D11Buffer> Shader::constantBuffers_[static_cast<size_t>(ConstantBuffer::MAX)];

// --- �t���[���o�b�t�@ ---
std::shared_ptr<FrameBuffer> Shader::frameBuffers_[static_cast<size_t>(FrameBufferLabel::MAX)];

// --- �V���h�E�}�b�v ---
std::unique_ptr<ShadowMap> Shader::shadowMap_;
// --- �s�N�Z���V�F�[�_�[ ---
Microsoft::WRL::ComPtr<ID3D11PixelShader> Shader::pixelShaders_[static_cast<size_t>(PixelShader::MAX)];

// --- ���_�V�F�[�_�[ ---
Microsoft::WRL::ComPtr<ID3D11VertexShader> Shader::vertexShaders_[static_cast<size_t>(VertexShader::MAX)];

std::mutex Shader::mutex_;



// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
void Shader::CreateIndexBuffer(ID3D11Device* device, ID3D11Buffer** ib, UINT byteSize, uint32_t* pData)
{
	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};

	bufferDesc.ByteWidth = byteSize;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	subResourceData.pSysMem = pData;

	HRESULT hr = device->CreateBuffer(
		&bufferDesc,
		&subResourceData,
		ib
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- ���_�V�F�[�_�[�̍쐬 ---
void Shader::CreateVSFromCSO(
	ID3D11Device* device,
	const char* CSOName,
	ID3D11VertexShader** vertexShader
)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- ���_�V�F�[�_�[�̍쐬 ---
	CreateVS(device, CSOData.get(), static_cast<size_t>(CSOSize), vertexShader);
}


// --- ���_�V�F�[�_�[�Ɠ��̓��C�A�E�g�̍쐬 ---
void Shader::CreateVSAndILFromCSO(
	ID3D11Device* device,
	const char* CSOName,
	ID3D11VertexShader** vertexShader,
	ID3D11InputLayout** inputLayout,
	D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
	UINT numElements
)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- ���_�V�F�[�_�[�̍쐬 ---
	CreateVS(device, CSOData.get(), static_cast<size_t>(CSOSize), vertexShader);

	// --- ���̓��C�A�E�g�̍쐬 ---
	CreateIL(device, inputElementDesc, numElements, CSOData.get(), static_cast<size_t>(CSOSize), inputLayout);
}


// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
void Shader::CreatePSFromCSO(
	ID3D11Device* device,
	const char* CSOName,
	ID3D11PixelShader** pixelShader
)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	CreatePS(device, CSOData.get(), static_cast<size_t>(CSOSize), pixelShader);
}


// --- �W�I���g���V�F�[�_�[�̍쐬 ---
void Shader::CreateGSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11GeometryShader** geometryShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- �W�I���g���V�F�[�_�[�̍쐬 ---
	CreateGS(device, CSOData.get(), static_cast<size_t>(CSOSize), geometryShader);
}


// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
void Shader::CreateCSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11ComputeShader** computeShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSO�t�@�C���̓ǂݍ��� ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
	CreateCS(device, CSOData.get(), static_cast<size_t>(CSOSize), computeShader);
}


// --- �萔�o�b�t�@�̍쐬 ---
void Shader::CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** cb, UINT byteSize)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byteSize + ((byteSize % 16 == 0) ? 0 : 16 - (byteSize % 16));	//	�o�b�t�@�̃o�C�g�� ( 16�̔{���ɒ��� )
	desc.Usage = D3D11_USAGE_DEFAULT;												//	�o�b�t�@�̎g�p�@
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;									//	�o�b�t�@�̃o�C���h�t���O
	desc.CPUAccessFlags = 0;														//	CPU�̃A�N�Z�X�t���O
	desc.MiscFlags = 0;																//	���̑��̃t���O
	desc.StructureByteStride = 0;													//	�\���̂̃o�C�g�X�g���C�h

	// --- �o�b�t�@�̍쐬 ---
	HRESULT hr = device->CreateBuffer(
		&desc,
		nullptr,
		cb
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}



// --- �e�N�X�`���̓ǂݍ��� ---
void Shader::LoadTextureFromFile(
	ID3D11Device* device,
	const wchar_t* fileName,
	ID3D11ShaderResourceView** srv,
	D3D11_TEXTURE2D_DESC* tex2DDesc
)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	// --- dds�t�@�C�������p�Ɋg���q��ύX ---
	std::filesystem::path ddsFileName(fileName);
	ddsFileName.replace_extension("dds");


	// --- dds�t�@�C�������������� ---
	if (std::filesystem::exists(ddsFileName.c_str()))
	{
		hr = DirectX::CreateDDSTextureFromFile(
			device,
			ddsFileName.c_str(),
			resource.GetAddressOf(),
			srv
		);

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	// --- �@������Ȃ�������png�œǂݍ��� ---
	else
	{
		hr = DirectX::CreateWICTextureFromFile(
			device,
			fileName,
			resource.GetAddressOf(),
			srv
		);

		// --- �ǂݍ��߂Ȃ������� ---
		if (FAILED(hr))
		{
			// --- ���F�̃e�N�X�`�����쐬 ---
			hr = MakeDummyTexture(device, srv, 0xFFFFFFFF, 16);
		}

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- Texture2DDesc�̎擾 ---
	if (tex2DDesc && resource)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(tex2D.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		tex2D->GetDesc(tex2DDesc);
	}
}


// --- �_�~�[�e�N�X�`���̍쐬 ---
HRESULT Shader::MakeDummyTexture(
	ID3D11Device* device,
	ID3D11ShaderResourceView** shaderResourceView,
	DWORD value,
	UINT dimension
)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2dDesc = {};
	texture2dDesc.Width = dimension;
	texture2dDesc.Height = dimension;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;


	size_t texels = dimension * dimension;
	std::unique_ptr<DWORD[]> sysmem{ std::make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = sysmem.get();
	subresourceData.SysMemPitch = sizeof(DWORD) * dimension;


	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;

	hr = device->CreateTexture2D(
		&texture2dDesc,
		&subresourceData,
		texture2d.GetAddressOf()
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = device->CreateShaderResourceView(
		texture2d.Get(),
		&shaderResourceViewDesc,
		shaderResourceView
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return hr;
}



// --- �K�E�X�u���[�p�̏d�݃e�[�u���̌v�Z ---
void Shader::CalcWeightsTableFromGaussian(
	float* weightsTable,
	int sizeOfWeightTable,
	float sigma
)
{
	// --- �d�݂̍��v ---
	float total = 0.0f;

	for (size_t x/*��e�N�Z������̋���*/ = 0; x < sizeOfWeightTable; x++)
	{
		weightsTable[x] = expf(-0.5f * static_cast<float>(x * x) / sigma);
		total += 2.0f * weightsTable[x];
	}

	// --- �d�݂̍��v���P�� ---
	for (size_t i = 0; i < sizeOfWeightTable; i++)
	{
		weightsTable[i] /= total;
	}
}



// --- CSO�t�@�C���̓ǂݍ��� ---
void Shader::LoadCSOFile(
	const char* CSOName,
	std::unique_ptr<unsigned char[]>& CSOData,
	long& CSOSize
)
{
	FILE* fp = nullptr;
	fopen_s(&fp, CSOName, "rb");
	_ASSERT_EXPR_A(fp, L"CSO�t�@�C����������܂���ł���");

	fseek(fp, 0, SEEK_END);
	CSOSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	CSOData = std::make_unique<unsigned char[]>(CSOSize);
	fread(CSOData.get(), CSOSize, 1, fp);
	fclose(fp);
}


// --- ���_�V�F�[�_�[�̍쐬 ---
void Shader::CreateVS(
	ID3D11Device* device,
	const unsigned char* CSOData,
	size_t CSOSize,
	ID3D11VertexShader** vertexShader
)
{
	HRESULT hr = device->CreateVertexShader(
			CSOData,
		CSOSize,
		nullptr,
		vertexShader
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
void Shader::CreatePS(
	ID3D11Device* device,
	const unsigned char* CSOData, size_t CSOSize,
	ID3D11PixelShader** pixelShader
)
{
	HRESULT hr = device->CreatePixelShader(
		CSOData,
		CSOSize,
		nullptr,
		pixelShader
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- �W�I���g���V�F�[�_�[�̍쐬 ---
void Shader::CreateGS(
	ID3D11Device* device,
	const unsigned char* CSOData,
	size_t CSOSize,
	ID3D11GeometryShader** geometryShader
)
{
	HRESULT hr = device->CreateGeometryShader(
		CSOData,
		CSOSize, 
		nullptr, 
		geometryShader
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

}


// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
void Shader::CreateCS(
	ID3D11Device* device, 
	const unsigned char* CSOData, 
	size_t CSOSize, 
	ID3D11ComputeShader** computeShader
)
{
	HRESULT hr = device->CreateComputeShader(
		CSOData, 
		CSOSize,
		nullptr,
		computeShader
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}


// --- ���̓��C�A�E�g�̍쐬 ---
void Shader::CreateIL(
	ID3D11Device* device, 
	const D3D11_INPUT_ELEMENT_DESC* inputElementDesc, 
	UINT numElements, 
	const unsigned char* CSOData, 
	size_t CSOSize, 
	ID3D11InputLayout** inputLayout
)
{
	HRESULT hr = device->CreateInputLayout(
		inputElementDesc,
		numElements,
		CSOData,
		CSOSize,
		inputLayout
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}



// --- �t���[���o�b�t�@�̍쐬 ---
void Shader::CreateFrameBuffer(
	ID3D11Device* device,
	uint32_t width,
	uint32_t height,
	DXGI_FORMAT format,
	size_t index
)
{
	frameBuffers_[static_cast<size_t>(index)] = std::make_shared<FrameBuffer>(device, width, height, format);
}


// --- �|�X�g�G�t�F�N�g�p�̃V�F�[�_�[�쐬 ---
void Shader::CreatePostEffectShaders(ID3D11Device* device)
{
	// --- �s�N�Z���V�F�[�_�[ ---
	CreatePSFromCSO(device, "./Data/Shader/Monochrome.cso", pixelShaders_[static_cast<size_t>(PixelShader::MONOCHROME)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/AverageBlur.cso", pixelShaders_[static_cast<size_t>(PixelShader::AVERAGE_BLUR)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/GaussianBlur_PS.cso", pixelShaders_[static_cast<size_t>(PixelShader::GAUSSIAN_BLUR)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/LuminanceExtraction.cso", pixelShaders_[static_cast<size_t>(PixelShader::LUMINANCE_EXTRACTION)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/Bloom.cso", pixelShaders_[static_cast<size_t>(PixelShader::BLUR_SCENE)].GetAddressOf());

	// --- ���_�V�F�[�_�[ ---
	CreateVSFromCSO(device, "./Data/Shader/HorizontalBlur_VS.cso", vertexShaders_[static_cast<size_t>(VertexShader::HORIZONTAL_BLUR)].GetAddressOf());
	CreateVSFromCSO(device, "./Data/Shader/VerticalBlur_VS.cso", vertexShaders_[static_cast<size_t>(VertexShader::VERTICAL_BLUR)].GetAddressOf());
}


// --- �V���h�E�}�b�v�̍쐬 ---
void Shader::CreateShadowMap(
	ID3D11Device* device,
	uint32_t width,
	uint32_t height
)
{
	shadowMap_ = std::make_unique<ShadowMap>(device, width, height);
}
