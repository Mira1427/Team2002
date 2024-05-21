#pragma once

#include <memory>
#include <mutex>

#include "../Library/Misc.h"

#include "../Math/Matrix.h"

#include "FrameBuffer.h"
#include "ShadowMap.h"


// --- �萔�o�b�t�@�̃��x�� ---
enum class ConstantBuffer
{
	SCENE,
	BLUR,
	MAX
};


//  --- �t���[���o�b�t�@�̃��x�� ---
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


// --- �s�N�Z���V�F�[�_�[�̃��x�� ---
enum class PixelShader
{
	MONOCHROME,
	AVERAGE_BLUR,
	GAUSSIAN_BLUR,
	LUMINANCE_EXTRACTION,
	BLUR_SCENE,
	MAX
};


// --- ���_�V�F�[�_�[�̃��x�� ---
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


	// --- �t���[���o�b�t�@�̍쐬 ---
	static void CreateFrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, DXGI_FORMAT format, size_t index);


	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	static void CreatePostEffectShaders(ID3D11Device* device);

	// --- �V���h�E�}�b�v�̍쐬 ---
	static void CreateShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);


	// --- ���_�o�b�t�@�̍쐬 ---
	template <typename T>
	static void CreateVertexBuffer(
		ID3D11Device* device,	// �f�o�C�X
		ID3D11Buffer** vb,		// �o�b�t�@
		UINT byteSize,			// �o�b�t�@�̃T�C�Y
		T* data,				// �o�b�t�@�̃f�[�^
		D3D11_USAGE usage,		// �o�b�t�@�̎g�p�@
		int accessFlag			// �A�N�Z�X�t���O
	)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subResourceData = {};
		{
			bufferDesc.ByteWidth			= byteSize;					// �o�b�t�@�̃T�C�Y
			bufferDesc.Usage				= usage;					// �o�b�t�@�̎g�p�@
			bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;	// �o�C���h�t���O ( �����ł͒��_�o�b�t�@ )
			bufferDesc.CPUAccessFlags		= accessFlag;				// �A�N�Z�X�t���O
			bufferDesc.MiscFlags			= 0;						// ���̑��̃t���O
			bufferDesc.StructureByteStride	= 0;						// �X�g���C�h

			subResourceData.pSysMem				= data;	// �o�b�t�@�̃f�[�^
			subResourceData.SysMemPitch			= 0;	// �s�b�`
			subResourceData.SysMemSlicePitch	= 0;	// �X���C�X�s�b�`

			// --- �쐬 ---
			HRESULT hr = device->CreateBuffer(
				&bufferDesc,
				&subResourceData,
				vb
			);

			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
		}

	}


	// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
	static void CreateIndexBuffer(
		ID3D11Device* device,
		ID3D11Buffer** ib,
		UINT byteSize,
		uint32_t* pData
	);


	// --- ���_�V�F�[�_�[�̍쐬 ---
	static void CreateVSFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11VertexShader** vertexShader
	);


	// --- ���_�V�F�[�_�[�Ɠ��̓��C�A�E�g�̍쐬 ---
	static void CreateVSAndILFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11VertexShader** vertexShader,
		ID3D11InputLayout** inputLayout,
		D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements
	);


	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	static void CreatePSFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11PixelShader** pixelShader
	);


	// --- �W�I���g���V�F�[�_�[�̍쐬 ---
	static void CreateGSFromCSO(
		ID3D11Device* device,
		const char* CSOName,
		ID3D11GeometryShader** geometryShader
	);


	// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
	static void CreateCSFromCSO(
		ID3D11Device* device, 
		const char* CSOName, 
		ID3D11ComputeShader** computeShader
	);


	// --- �萔�o�b�t�@�̍쐬 ---
	static void CreateConstantBuffer(
		ID3D11Device* device,
		ID3D11Buffer** cb,
		UINT byteSize
	);

	// --- �萔�o�b�t�@�̍X�V ---
	template <typename T>
	static void UpdateConstantBuffer(
		ID3D11DeviceContext* dc,
		ID3D11Buffer* cb,
		T& data
	)
	{
		dc->UpdateSubresource(cb, 0, 0, &data, 0, 0);
	}



	// --- �e�N�X�`���̓ǂݍ��� ---
	static void LoadTextureFromFile(
		ID3D11Device* device,
		const wchar_t* fileName,
		ID3D11ShaderResourceView** srv,
		D3D11_TEXTURE2D_DESC* tex2DDesc
	);


	// --- �_�~�[�e�N�X�`���̍쐬 ---
	static HRESULT MakeDummyTexture(
		ID3D11Device* device,
		ID3D11ShaderResourceView** shaderResourceView,
		DWORD value/*0xAABBGGRR*/,
		UINT dimension
	);


	// --- �K�E�X�u���[�p�̏d�݃e�[�u���̌v�Z ---
	static void CalcWeightsTableFromGaussian(
		float* weightsTable,
		int sizeOfWeightTable,
		float sigma
	);


private:
	// --- CSO�t�@�C���̓ǂݍ��� ---
	static void LoadCSOFile(
		const char* CSOName,
		std::unique_ptr<unsigned char[]>& CSOData,
		long& CSOSize
	);


	// --- ���_�V�F�[�_�[�̍쐬 ---
	static void CreateVS(
		ID3D11Device* device,
		const unsigned char* CSOData,
		size_t CSOSize,
		ID3D11VertexShader** vertexShader
	);


	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	static void CreatePS(
		ID3D11Device* device,
		const unsigned char* CSOData,
		size_t CSOSize,
		ID3D11PixelShader** pixelShader
	);


	// --- �W�I���g���V�F�[�_�[�̍쐬 ---
	static void CreateGS(
		ID3D11Device* device, 
		const unsigned char* CSOData,
		size_t CSOSize, 
		ID3D11GeometryShader** geometryShader
	);

	// --- �R���s���[�g�V�F�[�_�[�̍쐬 ---
	static void CreateCS(
		ID3D11Device* device, 
		const unsigned char* CSOData,
		size_t CSOSize, 
		ID3D11ComputeShader** computeShader
	);



	// --- ���̓��C�A�E�g�̍쐬 ---
	static void CreateIL(
		ID3D11Device* device,
		const D3D11_INPUT_ELEMENT_DESC* inputElementDesc,
		UINT numElements,
		const unsigned char* CSOData,
		size_t CSOSize,
		ID3D11InputLayout** inputLayout
	);


	// --- �萔�o�b�t�@ ---
	static Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers_[static_cast<size_t>(ConstantBuffer::MAX)];

	// --- �t���[���o�b�t�@ ---
	static std::shared_ptr<FrameBuffer> frameBuffers_[static_cast<size_t>(FrameBufferLabel::MAX)];

	// --- �s�N�Z���V�F�[�_�[ ---
	static Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders_[static_cast<size_t>(PixelShader::MAX)];

	// --- ���_�V�F�[�_�[ ---
	static Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaders_[static_cast<size_t>(VertexShader::MAX)];

	// --- �V���h�E�}�b�v ---
	static std::unique_ptr<ShadowMap> shadowMap_;
};


// --- �V�[���萔 ---
#define AMBIENT_LIGHT   struct AmbientLight   { Vector3 color_; float pad_; }
#define DIRECTION_LIGHT struct DirectionLight { Matrix ViewProjection_; Vector3 direction_;	float intensity_; Vector3 color_; float	pad_;  }
#define POINT_LIGHT		struct PointLight	  { Vector3 position_; float intensity_; Vector3 color_; float range_; }

#define POINT_LIGHT_MAX 32


struct SceneConstant
{
	Matrix	viewProjection_;	// �J�����s��
	Matrix  invViewProjection_;	// �J�����̋t�s��
	Vector4	cameraPosition_;	// �J�����̈ʒu
	Vector2	windowSize_;		// �E�B���h�E�̃T�C�Y

	float	gamma_	  = 2.5f;	// �K���}
	float	invGamma_ = 2.5f;	// �t�K���}

	AMBIENT_LIGHT	ambientLight_;					// �A���r�G���g���C�g
	DIRECTION_LIGHT directionLight_;				// �f�B���N�V�������C�g
	POINT_LIGHT		pointLights_[POINT_LIGHT_MAX];	// �|�C���g���C�g
};
