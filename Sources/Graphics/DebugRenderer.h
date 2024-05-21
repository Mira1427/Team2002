#pragma once

#include <wrl.h>
#include <list>

#include <d3d11.h>

#include "../Math/Matrix.h"


// ===== �f�o�b�O�v���~�e�B�u�`��N���X ===================================================================================================================
class DebugRenderer
{
public:
	// --- �R���X�g���N�^ ---
	DebugRenderer(ID3D11Device* device);


	/// <summary>
	/// --- �����`�̕`�� ---
	/// </summary>
	/// <param name="position">: �ʒu</param>
	/// <param name="size_">: �T�C�Y</param>
	/// <param name="rotation_">: ��]��</param>
	/// <param name="color_">: �F</param>
	/// <remarks>
	/// ���ۂɂ̓��X�g�ɒǉ����Ă��邾��
	/// </remarks>
	void DrawRectangle(const Vector2& position, const Vector2& size_, const Vector2& center = Vector2(0.0f, 0.0f), float rotation = 0.0f, const Vector4 & color_ = Vector4::Black_);


	/// <summary>
	/// --- �~�̕`�� ---
	/// </summary>
	/// <param name="position">: �ʒu</param>
	/// <param name="radius">: ���a</param>
	/// <param name="rotation">: ��]��</param>
	/// <param name="color">: �F</param>
	/// <remarks>
	/// ���ۂɂ̓��X�g�ɒǉ����Ă��邾��
	/// </remarks>
	void DrawCircle(const Vector2& position, float radius = 1.0f, const Vector4& color = Vector4::Black_);


	/// <summary>
	/// --- �����̂̕`�� ---
	/// </summary>
	/// <param name="position">: �ʒu</param>
	/// <param name="size">: �T�C�Y</param>
	/// <param name="rotation">: ��]��</param>
	/// <param name="color">: �F</param>
	/// <remarks>
	/// ���ۂɂ̓��X�g�ɒǉ����Ă��邾��
	/// </remarks>
	void DrawCube(const Vector3& position, const Vector3& size = Vector3::Unit_, const Vector3& rotation = Vector3::Zero_, const Vector4& color = Vector4::Black_);


	/// <summary>
	/// --- ���̕`�� ---
	/// </summary>
	/// <param name="position">: �ʒu</param>
	/// <param name="radius">: ���a</param>
	/// <param name="color">: �F</param>
	/// <remarks>
	/// ���ۂɂ̓��X�g�ɒǉ����Ă��邾��
	/// </remarks>
	void DrawSphere(const Vector3& position, float radius = 1.0f, const Vector4& color = Vector4::Black_);


	/// <summary>
	/// --- �~���̕`�� ---
	/// </summary>
	/// <param name="position">: �ʒu</param>
	/// <param name="radius">: ���a</param>
	/// <param name="height">: ����</param>
	/// <param name="rotation">: ��]��</param>
	/// <param name="color">: �F</param>
	/// <remarks>
	/// ���ۂɂ̓��X�g�ɒǉ����Ă��邾��
	/// </remarks>
	void DrawCapsule(const Vector3& position, float radius = 1.0f, float height = 1.0f, const Vector3& rotation = Vector3::Zero_, const Vector4& color = Vector4::Black_);


	// --- �����̕`�� ---
	void DrawLine(const Vector3& src, const Vector3& dst);


	// --- �`�� ---
	void Draw(ID3D11DeviceContext* immediateComtext);

private:
	// --- �����`�̍쐬 ---
	void CreateRectangle(ID3D11Device* device);

	// --- �~�̍쐬 ---
	void CreateCircle(ID3D11Device* device, int vertexNum);

	// --- �����̂̍쐬 ---
	void CreateCube(ID3D11Device* device);

	// --- ���̍쐬 ---
	void CreateSphere(ID3D11Device* device, int vertexNum);

	// --- �~���̍쐬 ---
	void CreateCapsule(ID3D11Device* device, int vertexNum);

	// --- �����̍쐬 ---
	void CreateLine(ID3D11Device* device);


	// --- �����`�̃I�u�W�F�N�g�f�[�^ ---
	struct Rectangle
	{
		Vector4 color_;
		Vector2 position_;
		Vector2 size_;
		Vector2 center_;
		float	rotation_;
	};
	std::list<Rectangle> rectangles_;								// �I�u�W�F�N�g�̃��X�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> rectangleVertexBuffer_;	// ���_�o�b�t�@


	// --- �~�̃I�u�W�F�N�g�f�[�^ ---
	struct Circle
	{
		Vector4 color_;
		Vector2 position_;
		float	radius_;
	};
	std::list<Circle> circles_;									// �I�u�W�F�N�g�̃��X�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> circleVertexBuffer_;	// ���_�o�b�t�@
	size_t circleVertexCount_;									// ���_�̐�


	// --- �����̂̃I�u�W�F�N�g�f�[�^
	struct Cube
	{
		Vector4 color_;
		Vector3 position_;
		Vector3 size_;
		Vector3 rotation_;
	};
	std::list<Cube> cubes_;									// �I�u�W�F�N�g�̃��X�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> cubeVertexBuffer_;	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> cubeIndexBuffer_;	// �C���f�b�N�X�o�b�t�@


	// --- ���̃I�u�W�F�N�g�f�[�^ ---
	struct Sphere
	{
		Vector4 color_;
		Vector3 position_;
		float	radius_;
	};
	std::list<Sphere> spheres_;									// �I�u�W�F�N�g�̃��X�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> sphereVertexBuffer_;	// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> sphereIndexBuffer_;	// �C���f�b�N�X�o�b�t�@


	// --- �~���̃I�u�W�F�N�g�f�[�^ ---
	struct Capsule
	{
		Vector4 color_;
		Vector3 position_;
		float	radius_;
		Vector3 rotation_;
		float	height_;
	};
	std::list<Capsule> capsules_;									// �I�u�W�F�N�g�̃��X�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> capsuleVertexBuffer_;		// ���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> capsuleIndexBuffer_;		// �C���f�b�N�X�o�b�t�@


	// --- �����̃I�u�W�F�N�g�f�[�^ ---
	struct Line
	{
		Vector3 src_;
		Vector3 dst_;
	};
	std::list<Line> lines_;									// �I�u�W�F�N�g�̃��X�g
	Microsoft::WRL::ComPtr<ID3D11Buffer> lineVertexBuffer_;	// ���_�o�b�t�@


	struct Constants3D
	{
		Matrix	world_;
		Vector4 color_;
	};

	struct Constants2D
	{
		Vector2	position_;
		Vector2	size_;
		Vector4	color_;
		Vector2	center_;
		float	rotation_;
	};


	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;		// ���_�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader2D_;	
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;		// �s�N�Z���V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;		// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer_;	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer2D_;
};

