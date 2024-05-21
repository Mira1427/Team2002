#include "Shader.h"

#include <filesystem>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>


// --- 定数バッファ ---
Microsoft::WRL::ComPtr<ID3D11Buffer> Shader::constantBuffers_[static_cast<size_t>(ConstantBuffer::MAX)];

// --- フレームバッファ ---
std::shared_ptr<FrameBuffer> Shader::frameBuffers_[static_cast<size_t>(FrameBufferLabel::MAX)];

// --- シャドウマップ ---
std::unique_ptr<ShadowMap> Shader::shadowMap_;
// --- ピクセルシェーダー ---
Microsoft::WRL::ComPtr<ID3D11PixelShader> Shader::pixelShaders_[static_cast<size_t>(PixelShader::MAX)];

// --- 頂点シェーダー ---
Microsoft::WRL::ComPtr<ID3D11VertexShader> Shader::vertexShaders_[static_cast<size_t>(VertexShader::MAX)];

std::mutex Shader::mutex_;



// --- インデックスバッファの作成 ---
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


// --- 頂点シェーダーの作成 ---
void Shader::CreateVSFromCSO(
	ID3D11Device* device,
	const char* CSOName,
	ID3D11VertexShader** vertexShader
)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSOファイルの読み込み ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- 頂点シェーダーの作成 ---
	CreateVS(device, CSOData.get(), static_cast<size_t>(CSOSize), vertexShader);
}


// --- 頂点シェーダーと入力レイアウトの作成 ---
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

	// --- CSOファイルの読み込み ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- 頂点シェーダーの作成 ---
	CreateVS(device, CSOData.get(), static_cast<size_t>(CSOSize), vertexShader);

	// --- 入力レイアウトの作成 ---
	CreateIL(device, inputElementDesc, numElements, CSOData.get(), static_cast<size_t>(CSOSize), inputLayout);
}


// --- ピクセルシェーダーの作成 ---
void Shader::CreatePSFromCSO(
	ID3D11Device* device,
	const char* CSOName,
	ID3D11PixelShader** pixelShader
)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSOファイルの読み込み ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- ピクセルシェーダーの作成 ---
	CreatePS(device, CSOData.get(), static_cast<size_t>(CSOSize), pixelShader);
}


// --- ジオメトリシェーダーの作成 ---
void Shader::CreateGSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11GeometryShader** geometryShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSOファイルの読み込み ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- ジオメトリシェーダーの作成 ---
	CreateGS(device, CSOData.get(), static_cast<size_t>(CSOSize), geometryShader);
}


// --- コンピュートシェーダーの作成 ---
void Shader::CreateCSFromCSO(ID3D11Device* device, const char* CSOName, ID3D11ComputeShader** computeShader)
{
	std::unique_ptr<unsigned char[]> CSOData;
	long CSOSize{};

	// --- CSOファイルの読み込み ---
	LoadCSOFile(CSOName, CSOData, CSOSize);

	// --- コンピュートシェーダーの作成 ---
	CreateCS(device, CSOData.get(), static_cast<size_t>(CSOSize), computeShader);
}


// --- 定数バッファの作成 ---
void Shader::CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** cb, UINT byteSize)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = byteSize + ((byteSize % 16 == 0) ? 0 : 16 - (byteSize % 16));	//	バッファのバイト幅 ( 16の倍数に調整 )
	desc.Usage = D3D11_USAGE_DEFAULT;												//	バッファの使用法
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;									//	バッファのバインドフラグ
	desc.CPUAccessFlags = 0;														//	CPUのアクセスフラグ
	desc.MiscFlags = 0;																//	その他のフラグ
	desc.StructureByteStride = 0;													//	構造体のバイトストライド

	// --- バッファの作成 ---
	HRESULT hr = device->CreateBuffer(
		&desc,
		nullptr,
		cb
	);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}



// --- テクスチャの読み込み ---
void Shader::LoadTextureFromFile(
	ID3D11Device* device,
	const wchar_t* fileName,
	ID3D11ShaderResourceView** srv,
	D3D11_TEXTURE2D_DESC* tex2DDesc
)
{
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	// --- ddsファイル検索用に拡張子を変更 ---
	std::filesystem::path ddsFileName(fileName);
	ddsFileName.replace_extension("dds");


	// --- ddsファイルが見つかったら ---
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

	// --- 　見つからなかったらpngで読み込み ---
	else
	{
		hr = DirectX::CreateWICTextureFromFile(
			device,
			fileName,
			resource.GetAddressOf(),
			srv
		);

		// --- 読み込めなかったら ---
		if (FAILED(hr))
		{
			// --- 白色のテクスチャを作成 ---
			hr = MakeDummyTexture(device, srv, 0xFFFFFFFF, 16);
		}

		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}


	// --- Texture2DDescの取得 ---
	if (tex2DDesc && resource)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(tex2D.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		tex2D->GetDesc(tex2DDesc);
	}
}


// --- ダミーテクスチャの作成 ---
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



// --- ガウスブラー用の重みテーブルの計算 ---
void Shader::CalcWeightsTableFromGaussian(
	float* weightsTable,
	int sizeOfWeightTable,
	float sigma
)
{
	// --- 重みの合計 ---
	float total = 0.0f;

	for (size_t x/*基準テクセルからの距離*/ = 0; x < sizeOfWeightTable; x++)
	{
		weightsTable[x] = expf(-0.5f * static_cast<float>(x * x) / sigma);
		total += 2.0f * weightsTable[x];
	}

	// --- 重みの合計を１に ---
	for (size_t i = 0; i < sizeOfWeightTable; i++)
	{
		weightsTable[i] /= total;
	}
}



// --- CSOファイルの読み込み ---
void Shader::LoadCSOFile(
	const char* CSOName,
	std::unique_ptr<unsigned char[]>& CSOData,
	long& CSOSize
)
{
	FILE* fp = nullptr;
	fopen_s(&fp, CSOName, "rb");
	_ASSERT_EXPR_A(fp, L"CSOファイルが見つかりませんでした");

	fseek(fp, 0, SEEK_END);
	CSOSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	CSOData = std::make_unique<unsigned char[]>(CSOSize);
	fread(CSOData.get(), CSOSize, 1, fp);
	fclose(fp);
}


// --- 頂点シェーダーの作成 ---
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


// --- ピクセルシェーダーの作成 ---
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


// --- ジオメトリシェーダーの作成 ---
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


// --- コンピュートシェーダーの作成 ---
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


// --- 入力レイアウトの作成 ---
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



// --- フレームバッファの作成 ---
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


// --- ポストエフェクト用のシェーダー作成 ---
void Shader::CreatePostEffectShaders(ID3D11Device* device)
{
	// --- ピクセルシェーダー ---
	CreatePSFromCSO(device, "./Data/Shader/Monochrome.cso", pixelShaders_[static_cast<size_t>(PixelShader::MONOCHROME)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/AverageBlur.cso", pixelShaders_[static_cast<size_t>(PixelShader::AVERAGE_BLUR)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/GaussianBlur_PS.cso", pixelShaders_[static_cast<size_t>(PixelShader::GAUSSIAN_BLUR)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/LuminanceExtraction.cso", pixelShaders_[static_cast<size_t>(PixelShader::LUMINANCE_EXTRACTION)].GetAddressOf());
	CreatePSFromCSO(device, "./Data/Shader/Bloom.cso", pixelShaders_[static_cast<size_t>(PixelShader::BLUR_SCENE)].GetAddressOf());

	// --- 頂点シェーダー ---
	CreateVSFromCSO(device, "./Data/Shader/HorizontalBlur_VS.cso", vertexShaders_[static_cast<size_t>(VertexShader::HORIZONTAL_BLUR)].GetAddressOf());
	CreateVSFromCSO(device, "./Data/Shader/VerticalBlur_VS.cso", vertexShaders_[static_cast<size_t>(VertexShader::VERTICAL_BLUR)].GetAddressOf());
}


// --- シャドウマップの作成 ---
void Shader::CreateShadowMap(
	ID3D11Device* device,
	uint32_t width,
	uint32_t height
)
{
	shadowMap_ = std::make_unique<ShadowMap>(device, width, height);
}
