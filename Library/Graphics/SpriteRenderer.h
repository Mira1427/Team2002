#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Matrix.h"


struct Texture;

class SpriteRenderer
{
public:
	// --- �R���X�g���N�^ ---
	SpriteRenderer(ID3D11Device* device);

	// --- �`�揈�� ---
	void Draw(
		ID3D11DeviceContext* dc,
		ID3D11ShaderResourceView** srv,
		const float texWidth,						// ���摜�̍���
		const float texHeight,						// ���摜�̕�
		const Vector3& position,					// �ʒu
		const Vector3& scale,						// �X�P�[��
		const Vector2& texPos,						// �؂蔲���ʒu
		const Vector2& texSize,						// �؂蔲���T�C�Y
		const Vector2& center = Vector2::Zero_,		// ���S
		const Vector3& rotation = Vector3::Zero_,	// ��]��
		const Vector4& color = Vector4::White_,		// �F
		bool inWorld = false,						// ���[���h��Ԃɕ`�悷�邩
		bool useBillboard = false,					// �r���{�[�h���g�p���邩
		ID3D11PixelShader* replacementPS = nullptr,	// �u��������s�N�Z���V�F�[�_�[
		ID3D11VertexShader* replacementVS = nullptr,// �u�������钸�_�V�F�[�_�[
		UINT startSlot = 0,							// �e�N�X�`�����Z�b�g����ꏊ
		UINT numViews = 1							// �Z�b�g�����
	);


	void Draw(
		ID3D11DeviceContext* dc,
		Texture& texture,
		const Vector3& position,
		const Vector3& scale,
		const Vector2& texPos,
		const Vector2& texSize,
		const Vector2& center = Vector2::Zero_,
		const Vector3& rotation = Vector3::Zero_,
		const Vector4& color = Vector4::White_,
		bool inWorld = false,
		bool useBillboard = false,
		ID3D11PixelShader* replacementPS = nullptr,
		ID3D11VertexShader* replacementVS = nullptr,
		UINT startSlot = 0,
		UINT numViews = 1
	);


	// --- 2D�I�u�W�F�N�g�̒萔�o�b�t�@�\���� ---
	struct ObjectConstants2D
	{
		Vector2 position_;
		Vector2 size_;
		Vector4 color_;
		Vector2 center_;
		Vector2 scale_;
		Vector2 texPos_;
		Vector2 texSize_;
		float	rotation_;
	};

	// --- 3D�I�u�W�F�N�g�̒萔�o�b�t�@�\���� ---
	struct ObjectConstants3D
	{
		Matrix  world_;
		Vector4 color_;
		Vector2 offset_;
		Vector2 size_;
		Vector2 texPos_;
		Vector2 texSize_;
	};

	// --- ���_�f�[�^�\���� ---
	struct Vertex
	{
		Vector3 position_;
		Vector2 texcoord_;
	};

	// --- �V�F�[�_�[�̃��\�[�X�\���� ---
	struct ShaderResources
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer_;
	};

	enum
	{
		SPRITE2D,
		SPRITE3D,
	};

private:
	ShaderResources shaderResources_[2];
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
};

