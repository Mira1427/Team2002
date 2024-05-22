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


	ID3D11Device*			GetDevice()			{ return device_.Get(); }			// �f�o�C�X�̎擾
	ID3D11DeviceContext*	GetDeviceContext()	{ return immediateContext_.Get(); }	// �f�o�C�X�R���e�L�X�g�̎擾
	IDXGISwapChain*			GetSwapChain()		{ return swapChain_.Get(); }		// �X���b�v�`�F�C���̎擾


	Primitive*			GetPrimitiveRenderer()	{ return primitiveRenderer_.get(); }	// �v���~�e�B�u�`��N���X
	SpriteRenderer*		GetSpriteRenderer()		{ return spriteRenderer_.get(); }		// �X�v���C�g�`��N���X
	DebugLineRenderer*	GetDebugLineRenderer()	{ return debugLineRenderer_.get(); }	// �f�o�b�O���C���`��N���X
	DebugRenderer*		GetDebugRenderer()		{ return debugRenderer_.get(); }		// �f�o�b�O�`��N���X
	SkyMap*				GetSkyMapRenderer()		{ return skyMapRenderer_.get(); }		// �X�J�C�}�b�v�`��N���X

	LightingManager* GetLightingManager() { return lightingManager_.get(); }	// ���C�g�Ǘ��N���X


	// --- ������ ---
	void Initialize(
		HWND hwnd,		// �E�B���h�E�n���h��
		LONG width,		// ��
		LONG height,	// ����
		BOOL isWindowed	// �t���X�N���[����
	);

	// --- �`��N���X�̏����� ---
	void InitializeRenderer();

	// --- �Ǘ��N���X�̏����� ---
	void InitializeManager();


	// --- �����_�[�^�[�Q�b�g�r���[�ƃf�v�X�X�e���V���r���[�̃N���A ---
	void ClearRTVAndDSV(const Vector4& color);

	// --- �����_�[�^�[�Q�b�g�̐ݒ� ---
	void SetRenderTarget();

	// --- ��̃����_�[�^�[�Q�b�g�r���[�̐ݒ� ---
	void SetNullRTV();

	// --- ��̃V�F�[�_�[���\�[�X�r���[�̍쐬 ---
	void SetNullSRV();


	// --- �u�����h�X�e�[�g�̐ݒ� ---
	void SetBlendState(BlendState state)
	{ immediateContext_->OMSetBlendState(blendStates_[static_cast<size_t>(state)].Get(), nullptr, 0xFFFFFFFF); }

	// --- �T���v���[�X�e�[�g�̐ݒ� ---
	void SetSamplerState()
	{
		immediateContext_->PSSetSamplers(0, 1, samplerStates_[static_cast<size_t>(SamplerState::POINT)].GetAddressOf());
		immediateContext_->PSSetSamplers(1, 1, samplerStates_[static_cast<size_t>(SamplerState::LINEAR)].GetAddressOf());
		immediateContext_->PSSetSamplers(2, 1, samplerStates_[static_cast<size_t>(SamplerState::ANISOTROPIC)].GetAddressOf());
		immediateContext_->PSSetSamplers(3, 1, samplerStates_[static_cast<size_t>(SamplerState::POINT_ADDRESS_BODER)].GetAddressOf());
		immediateContext_->PSSetSamplers(4, 1, samplerStates_[static_cast<size_t>(SamplerState::SHADOW)].GetAddressOf());
	}

	// --- �[�x�X�e���V���̐ݒ� ---
	void SetDepthStencilState(DepthState test, DepthState write)
	{ immediateContext_->OMSetDepthStencilState(depthStencilStates_[static_cast<size_t>(test)][static_cast<size_t>(write)].Get(), 1); }

	void SetRasterState(RasterState state)
	{ immediateContext_->RSSetState(rasterStates_[static_cast<size_t>(state)].Get()); }


	// --- �`��̑O���� ---
	void Begin(const Vector4& color);

	// --- �`��̌㏈�� ---
	HRESULT End(UINT syncInterval = 0U, UINT flags = 0U);


	std::mutex& GetMutex() { return mutex_; }


private:
	Microsoft::WRL::ComPtr<ID3D11Device>			device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext_;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView_;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		samplerStates_[static_cast<size_t>(SamplerState::MAX)];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates_[2/*�[�x�e�X�g*/][2/*�[�x��������*/];
	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendStates_[static_cast<size_t>(BlendState::MAX)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterStates_[static_cast<size_t>(RasterState::MAX)];
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffers_[8];


	std::unique_ptr<Primitive>			primitiveRenderer_;	// �v���~�e�B�u�`��N���X
	std::unique_ptr<SpriteRenderer>		spriteRenderer_;	// �X�v���C�g�`��N���X
	std::unique_ptr<DebugLineRenderer>	debugLineRenderer_;	// �f�o�b�O���C���`��N���X
	std::unique_ptr<LightingManager>	lightingManager_;	// ���C�g�Ǘ��N���X
	std::unique_ptr<DebugRenderer>		debugRenderer_;		// �f�o�b�O�`��N���X
	std::unique_ptr<SkyMap>				skyMapRenderer_;	// �X�J�C�}�b�v�`��N���X

	std::mutex mutex_;
};
