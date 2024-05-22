#pragma once

#include <memory>
#include <mutex>

#include "../Library/Misc.h"

#include "../Math/Matrix.h"

#include "FrameBuffer.h"
#include "ShadowMap.h"


// --- 定数バッファのラベル ---
enum class ConstantBuffer
{
	SCENE,
	BLUR,
	MAX
};


//  --- フレームバッファのラベル ---
enum class FrameBufferLabel
{
	SCENE,
	HORIZONTAL_BLUR,
	VERTICAL_BLUR,
	LUMINANCE_EXTRACTION,
	GAUSSIAN_BLUR00,
	GAUSSIAN_BLUR01,
	GAUSSIAN_BLUR02,
	GAUSSIAN_BLUR03,
	MAX
};


// --- ピクセルシェーダーのラベル ---
enum class PixelShader
{
	MONOCHROME,
	AVERAGE_BLUR,
	GAUSSIAN_BLUR,
	LUMINANCE_EXTRACTION,
	BLUR_SCENE,
	MAX
};


// --- 頂点シェーダーのラベル ---
enum class VertexShader
{
	HORIZONTAL_BLUR,
	VERTICAL_BLUR,
	MAX
};


class Shader
{
public:
	template<typename T>
	static ID3D11Buffer* GetConstantBuffer(T index) { return constantBuffers_[static_cast<size_t>(index)].Get(); }

	template<typename T>
	static ID3D11Buffer** GetCBAddress(T index) {return constantBuffers_[static_cast<size_t>(index)].GetAddressOf(); }

	template<typename T>
	static FrameBuffer* GetFrameBuffer(T index) { return frameBuffers_[static_cast<size_t>(index)].get(); }

	static ShadowMap* GetShadowMap() { return shadowMap_.get(); }
	template<typename T>
	static ID3D11PixelShader* GetPixelShader(T index) { return pixelShaders_[static_cast<size_t>(index)].Get(); }

	template<typename T>
	static ID3D11VertexShader* GetVertexShader(T index) { return vertexShaders_[static_cast<size_t>(index)].Get(); }


	// --- フレームバッファの作成 ---
	static void CreateFrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, size_t index);


	// --- ピクセルシェーダーの作成 ---
	static void CreatePostEffectShaders(ID3D11Device* device);

	// --- シャドウマップの作成 ---
	static void CreateShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);


	// --- 頂点バッファの作成 ---
	template <typename T>
	static void CreateVertexBuffer(
		ID3D11Device* device,	// デバイス
		ID3D11Buffer** vb,		// バッファ
		UINT byteSize,			// バッファのサイズ
		T* data,				// バッファのデータ
		D3D11_USAGE usage,		// バッファの使用法
		int accessFlag			// アクセスフラグ
	)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subResourceData = {};
		{
			bufferDesc.ByteWidth			= byteSize;					// バッファのサイズ
			bufferDesc.Usage				= usage;					// バッファの使用法
			bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// バインドフラグ ( ここでは頂点バッファ )
			bufferDesc.CPUAccessFlags		= accessFlag;				// アクセスフラグ
			bufferDesc.MiscFlags			= 0;						// その他のフラグ
			bufferDesc.StructureByteStride	= 0;						// ストライド

			subResourceData.pSysMem				= data;	// バッファのデータ
			subResourceData.SysMemPitch			= 0;	// ピッチ
			subResourceData.SysMemSlicePitch	= 0;	// スライスピッチ

			// --- 作成 ---
			HRESULT hr = device->CreateBuffer(
				&bufferDesc,
				&subResourceData,
				vb
			);

			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}

	}


	// --- インデックスバッファの作成 ---
	static void CreateIndexBuffer(
		ID3D11Device* device,
		ID3D11Buffer** ib,
		UINT byteSize,
		uint32_t* pData
	);


	// --- 頂点シェーダーの作成 ---
	static void CreateVSFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11VertexShader** vertexShader
	);


	// --- 頂点シェーダーと入力レイアウトの作成 ---
	static void CreateVSAndILFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements
	);


	// --- ピクセルシェーダーの作成 ---
	static void CreatePSFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11PixelShader** pixelShader
	);


	// --- ジオメトリシェーダーの作成 ---
	static void CreateGSFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11GeometryShader** geometryShader
	);


	// --- コンピュートシェーダーの作成 ---
	static void CreateCSFromCSO(
		ID3D11Device* device, 
		const char* CSOName, 
		ID3D11ComputeShader** computeShader
	);


	// --- 定数バッファの作成 ---
	static void CreateConstantBuffer(
		ID3D11Device* device,
		ID3D11Buffer** cb,
		UINT byteSize
	);

	// --- 定数バッファの更新 ---
	template <typename T>
	static void UpdateConstantBuffer(
		ID3D11DeviceContext* dc,
		ID3D11Buffer* cb,
		T& data
	)
	{
		dc->UpdateSubresource(cb, 0, 0, &data, 0, 0);
	}



	// --- テクスチャの読み込み ---
	static void LoadTextureFromFile(
		ID3D11Device* device,
		const wchar_t* fileName,
		ID3D11ShaderResourceView** srv,
		D3D11_TEXTURE2D_DESC* tex2DDesc
	);


	// --- ダミーテクスチャの作成 ---
	static HRESULT MakeDummyTexture(
		ID3D11Device* device,
		ID3D11ShaderResourceView** shaderResourceView,
		DWORD value/*0xAABBGGRR*/,
		UINT dimension
	);


	// --- ガウスブラー用の重みテーブルの計算 ---
	static void CalcWeightsTableFromGaussian(
		float* weightsTable,
		int sizeOfWeightTable,
		float sigma
	);


private:
	// --- CSOファイルの読み込み ---
	static void LoadCSOFile(
		const char* CSOName,
		std::unique_ptr<unsigned char[]>& CSOData,
		long& CSOSize
	);


	// --- 頂点シェーダーの作成 ---
	static void CreateVS(
		ID3D11Device* device,
		const unsigned char* CSOData,
		size_t CSOSize,
		ID3D11VertexShader** vertexShader
	);


	// --- ピクセルシェーダーの作成 ---
	static void CreatePS(
		ID3D11Device* device,
		const unsigned char* CSOData,
		size_t CSOSize,
		ID3D11PixelShader** pixelShader
	);


	// --- ジオメトリシェーダーの作成 ---
	static void CreateGS(
		ID3D11Device* device, 
		const unsigned char* CSOData,
		size_t CSOSize, 
		ID3D11GeometryShader** geometryShader
	);

	// --- コンピュートシェーダーの作成 ---
	static void CreateCS(
		ID3D11Device* device, 
		const unsigned char* CSOData,
		size_t CSOSize, 
		ID3D11ComputeShader** computeShader
	);



	// --- 入力レイアウトの作成 ---
	static void CreateIL(
		ID3D11Device* device,
		const D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements,
		const unsigned char* CSOData,
		size_t CSOSize,
		ID3D11InputLayout** inputLayout
	);


	// --- 定数バッファ ---
	static Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers_[static_cast<size_t>(ConstantBuffer::MAX)];

	// --- フレームバッファ ---
	static std::shared_ptr<FrameBuffer> frameBuffers_[static_cast<size_t>(FrameBufferLabel::MAX)];

	// --- ピクセルシェーダー ---
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders_[static_cast<size_t>(PixelShader::MAX)];

	// --- 頂点シェーダー ---
	static Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaders_[static_cast<size_t>(VertexShader::MAX)];

	// --- シャドウマップ ---
	static std::unique_ptr<ShadowMap> shadowMap_;
};


// --- シーン定数 ---
#define AMBIENT_LIGHT   struct AmbientLight   { Vector3 color_; float pad_; }
#define DIRECTION_LIGHT struct DirectionLight { Matrix ViewProjection_; Vector3 direction_;	float intensity_; Vector3 color_; float	pad_;  }
#define POINT_LIGHT		struct PointLight	  { Vector3 position_; float intensity_; Vector3 color_; float range_; }

#define POINT_LIGHT_MAX 32


struct SceneConstant
{
	Matrix	viewProjection_;	// カメラ行列
	Matrix  invViewProjection_;	// カメラの逆行列
	Vector4	cameraPosition_;	// カメラの位置
	Vector2	windowSize_;		// ウィンドウのサイズ

	float	gamma_	  = 2.5f;	// ガンマ
	float	invGamma_ = 2.5f;	// 逆ガンマ

	AMBIENT_LIGHT	ambientLight_;					// アンビエントライト
	DIRECTION_LIGHT directionLight_;				// ディレクションライト
	POINT_LIGHT		pointLights_[POINT_LIGHT_MAX];	// ポイントライト
};
