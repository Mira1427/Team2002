#include "DebugRenderer.h"

#include <vector>

#include "../Library/Misc.h"

#include "Graphics.h"
#include "Shader.h"


// --- �R���X�g���N�^ ---
DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// --- ���̓��C�A�E�g�̍쐬 ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// --- �V�F�[�_�[�̓ǂݍ��� ---
	Shader::CreateVSAndILFromCSO(device, "./Data/Shader/DebugRenderer_VS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Shader::CreateVSFromCSO(device, "./Data/Shader/DebugRenderer2D_VS.cso", vertexShader2D_.GetAddressOf());
	Shader::CreatePSFromCSO(device, "./Data/Shader/DebugRenderer_PS.cso", pixelShader_.GetAddressOf());

	Shader::CreateConstantBuffer(device, constantBuffer_.GetAddressOf(), sizeof(Constants3D));
	Shader::CreateConstantBuffer(device, constantBuffer2D_.GetAddressOf(), sizeof(Constants2D));
	
	// --- �����`�̍쐬 ---
	CreateRectangle(device);

	// --- �~�̍쐬 ---
	CreateCircle(device, 18);

	// --- �����̂̍쐬 ---
	CreateCube(device);

	// --- ���̍쐬 ---
	CreateSphere(device, 18);

	// --- �~���̍쐬 ---
	CreateCapsule(device, 18);

	// --- �����̍쐬 ---
	CreateLine(device);
}


// ===== �����` ============================================================================================================================================

// --- �쐬 ---
void DebugRenderer::CreateRectangle(ID3D11Device* device)
{
	// --- ���_���̍쐬 ---
	Vector3 vertices[5]
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
	};

	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(device, rectangleVertexBuffer_.GetAddressOf(), static_cast<UINT>(sizeof(vertices)), vertices, D3D11_USAGE_DEFAULT, 0);
}

// --- �`�� ---
void DebugRenderer::DrawRectangle(const Vector2& position, const Vector2& size, const Vector2& center, float rotation, const Vector4& color)
{
	Rectangle rectangle;
	rectangle.position_ = position;
	rectangle.size_		= size;
	rectangle.center_	= center;
	rectangle.rotation_ = rotation;
	rectangle.color_	= color;
	rectangles_.emplace_back(rectangle);
}


// ===== �~ =================================================================================================================================================

// --- �쐬 ---
void DebugRenderer::CreateCircle(ID3D11Device* device, int vertexNum)
{
	// --- ���_���̍쐬 ---
	std::vector<Vector3> vertices;
	for (int i = 0; i < vertexNum + 1; i++)
	{
		float radian = DirectX::XMConvertToRadians((360.0f / vertexNum) * i);
		Vector3 vertex = { cosf(radian), sinf(radian), 0.0f };
		vertices.emplace_back(vertex);
	}

	// --- ���_�̐� ---
	circleVertexCount_ = vertices.size();

	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(device, circleVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);
}

// --- �`�� ---
void DebugRenderer::DrawCircle(const Vector2& position, float radius, const Vector4& color)
{
	Circle circle;
	circle.position_ = position;
	circle.radius_ = radius;
	circle.color_ = color;
	circles_.emplace_back(circle);
}


// ===== ������ ============================================================================================================================================

// --- �쐬 ---
void DebugRenderer::CreateCube(ID3D11Device* device)
{	
	// --- ���_���̍쐬 ---
	std::vector<Vector3> vertices =	{
		// --- ��O ---
		{ -0.5f,  0.5f, -0.5f},	// ����
		{  0.5f,  0.5f, -0.5f},	// �E��
		{ -0.5f, -0.5f, -0.5f},	// ����
		{  0.5f, -0.5f, -0.5f},	// �E��
		// --- �� ---
		{ -0.5f,  0.5f, 0.5f},	// ����
		{  0.5f,  0.5f, 0.5f},	// �E��
		{ -0.5f, -0.5f, 0.5f},	// ����
		{  0.5f, -0.5f, 0.5f},	// �E��
	};

	// --- ���_�̃C���f�b�N�X ---
	uint32_t indices[]
	{
		0, 1, 1, 3, 3, 2, 2, 0, 0, 4, 1, 5, 3, 7, 2, 6, 4, 5, 5, 7, 7, 6, 6, 4
	};


	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};
	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(device, cubeVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
	Shader::CreateIndexBuffer(device, cubeIndexBuffer_.GetAddressOf(), static_cast<UINT>(sizeof(indices)), indices);
}

// --- �`�� ---
void DebugRenderer::DrawCube(const Vector3& position, const Vector3& size, const Vector3& rotation, const Vector4& color)
{
	Cube cube;
	cube.position_ = position;
	cube.size_ = size;
	cube.rotation_ = rotation;
	cube.rotation_.ToRadian();
	cube.color_ = color;
	cubes_.emplace_back(cube);
}


// ===== �� =================================================================================================================================================

// --- �쐬 ---
void DebugRenderer::CreateSphere(ID3D11Device* device, int vertexNum)
{
	// --- ���_���̍쐬 ---
	std::vector<Vector3> vertices;

	vertices.emplace_back(Vector3::Up_);

	for (int i = 1; i < static_cast<int>(vertexNum * 0.5f); i++)
	{
		float theta = DirectX::XMConvertToRadians((360.0f / vertexNum) * i + 90.0f);

		for (int j = 0; j < vertexNum; j++)
		{
			float radius = 1.0f * cosf(theta);
			float y = 1.0f * sinf(theta);
			float theta2 = DirectX::XMConvertToRadians((360.0f / vertexNum) * j + 90.0f);
			Vector3 vertex = { cosf(theta2) * radius, y, sinf(theta2) * radius };
			vertices.emplace_back(vertex);
		}
	}

	vertices.emplace_back(Vector3::Down_);


	// --- �C���f�b�N�X�̐ݒ� ---
	std::vector<uint32_t> indices;
	int layer = static_cast<int>(vertexNum * 0.5f) - 1;
	for (int i = 0; i < layer; i++)
	{
		for (int j = 0; j < vertexNum; j++)
		{
			indices.emplace_back(vertexNum * i + j + 1);
			indices.emplace_back(((j + 2 > vertexNum) ? 1 + i * vertexNum : vertexNum * i + j + 2));
		}
	}

	for (int i = 1; i < vertexNum; i++)
	{
		indices.emplace_back(0);
		indices.emplace_back(i);

		for (int j = 0; j < layer - 1; j++)
		{
			indices.emplace_back(vertexNum * j + i);
			indices.emplace_back(vertexNum * (j + 1) + i);
		}

		indices.emplace_back(vertexNum * (layer - 1) + i);
		indices.emplace_back(vertexNum * layer + 1);
	}

	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer<Vector3>(device, sphereVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
	Shader::CreateIndexBuffer(device, sphereIndexBuffer_.GetAddressOf(), static_cast<UINT>(indices.size() * sizeof(uint32_t)), indices.data());
}

// --- �`�� ---
void DebugRenderer::DrawSphere(const Vector3& position, float radius, const Vector4& color)
{
	Sphere sphere;
	sphere.position_ = position;
	sphere.radius_ = radius;
	sphere.color_ = color;
	spheres_.emplace_back(sphere);
}


// ===== �~�� =============================================================================================================================

// --- �쐬 ---
void DebugRenderer::CreateCapsule(ID3D11Device* device, int vertexNum)
{
	// --- ���_���̍쐬 ---
	std::vector<Vector3> vertices;
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < vertexNum; i++)
		{
			float theta = DirectX::XMConvertToRadians((360.0f / vertexNum) * i);
			Vector3 vertex = { cosf(theta), static_cast<float>(j), sinf(theta) };
			vertices.emplace_back(vertex);
		}
	}

	// --- �C���f�b�N�X�̐ݒ� ---
	std::vector<uint32_t> indices;
	for(int j = 0; j < 2; j++)
	{
		for (int i = 0; i < vertexNum; i++)
		{
			indices.emplace_back(vertexNum * j + i);
			indices.emplace_back(((i + 1 > vertexNum - 1) ? vertexNum * j : vertexNum * j + i + 1));
		}
	}
	for (int i = 0; i < vertexNum; i++)
	{
		indices.emplace_back(i);
		indices.emplace_back(i + vertexNum);
	}


	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};
	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(device, capsuleVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	// --- �C���f�b�N�X�o�b�t�@�̍쐬 ---
	Shader::CreateIndexBuffer(device, capsuleIndexBuffer_.GetAddressOf(), static_cast<UINT>(indices.size() * sizeof(uint32_t)), indices.data());
}

// --- �`�� ---
void DebugRenderer::DrawCapsule(const Vector3& position, float radius, float height, const Vector3& rotation, const Vector4& color)
{
	Capsule Capsule;
	Capsule.position_ = position;
	Capsule.radius_ = radius;
	Capsule.height_ = height;
	Capsule.rotation_ = rotation;
	Capsule.rotation_.ToRadian();
	Capsule.color_ = color;
	capsules_.emplace_back(Capsule);
}


// ===== ���� ======================================================================================================================================================
void DebugRenderer::CreateLine(ID3D11Device* device)
{	
	// --- ���_���̍쐬 ---
	Vector3 vertices[2];

	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(
		device, 
		lineVertexBuffer_.GetAddressOf(),
		static_cast<UINT>(sizeof(vertices)),
		vertices,
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE
	);
}

// --- �`�� ---
void DebugRenderer::DrawLine(const Vector3& src, const Vector3& dst)
{
	Line line;
	line.src_ = src;
	line.dst_ = dst;
	lines_.emplace_back(line);
}


// --- �`�� ---
void DebugRenderer::Draw(ID3D11DeviceContext* dc)
{
	// --- �V�F�[�_�[�ݒ� ---
	dc->VSSetShader(vertexShader2D_.Get(), nullptr, 0);	// ���_�V�F�[�_�[
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);	// �s�N�Z���V�F�[�_�[
	dc->IASetInputLayout(inputLayout_.Get());			// ���̓��C�A�E�g

	// --- �萔�o�b�t�@�ݒ� ---
	dc->VSSetConstantBuffers(0, 1, constantBuffer2D_.GetAddressOf());	// �I�u�W�F�N�g�萔

	UINT indexCount{};

	// ===== �����`�̕`�� ==================================================================================================================================
	
	// --- �v���~�e�B�u�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// --- �o�b�t�@��ݒ� ---
	UINT stride{ sizeof(Vector3) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, rectangleVertexBuffer_.GetAddressOf(), &stride, &offset);

	// --- ���X�g�̕������`�� ---
	for (const Rectangle& rectangle : rectangles_)
	{
		// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
		Constants2D cb;
		cb.position_	= rectangle.position_;
		cb.size_		= rectangle.size_;
		cb.rotation_	= rectangle.rotation_;
		cb.center_		= rectangle.center_;
		cb.color_		= rectangle.color_;
		dc->UpdateSubresource(constantBuffer2D_.Get(), 0, 0, &cb, 0, 0);

		// --- �`�� ---
		dc->Draw(5, 0);
	}
	rectangles_.clear();


	// ===== �~�̕`�� =======================================================================================================================================

	// --- �v���~�e�B�u�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// --- �o�b�t�@��ݒ� ---
	dc->IASetVertexBuffers(0, 1, circleVertexBuffer_.GetAddressOf(), &stride, &offset);

	for(const Circle& circle : circles_)
	{
		// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
		Constants2D cb;
		cb.position_ = circle.position_;
		cb.size_ = Vector2(circle.radius_, circle.radius_);
		cb.color_ = circle.color_;
		dc->UpdateSubresource(constantBuffer2D_.Get(), 0, 0, &cb, 0, 0);

		// --- �`�� ---
		dc->Draw(static_cast<UINT>(circleVertexCount_), 0U);
	}
	circles_.clear();


	// ===== 3D�̕`��ݒ� =======================================================================================================================================
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);					// ���_�V�F�[�_�[
	dc->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());		// �I�u�W�F�N�g�萔


	// ===== �����̂̕`�� ==================================================================================================================================

	// --- �v���~�e�B�u�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- �o�b�t�@��ݒ� ---
	dc->IASetVertexBuffers(0, 1, cubeVertexBuffer_.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(cubeIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- �`�悷��񐔂��v�Z ---
	D3D11_BUFFER_DESC bufferDesc{};
	cubeIndexBuffer_->GetDesc(&bufferDesc);
	indexCount = bufferDesc.ByteWidth / sizeof(uint32_t);

	// --- ���X�g�̕������`�� ---
	for (const Cube& cube : cubes_)
	{
		// --- ���[���h�s��̍쐬 ---
		Matrix S;
		S.MakeScaling(cube.size_);
		Matrix R;
		R.MakeRotation(cube.rotation_);
		Matrix T;
		T.MakeTranslation(cube.position_);
		Matrix W = S * R * T;

		// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = cube.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		// --- �`�� ---
		dc->DrawIndexed(indexCount, 0, 0);
	}
	cubes_.clear();


	// ===== ���̕`�� =======================================================================================================================================

	// --- �v���~�e�B�u�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- �o�b�t�@��ݒ� ---
	dc->IASetVertexBuffers(0, 1, sphereVertexBuffer_.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(sphereIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- �`�悷��񐔂��v�Z ---
	sphereIndexBuffer_->GetDesc(&bufferDesc);
	indexCount = bufferDesc.ByteWidth / sizeof(uint32_t);

	for (const Sphere& sphere : spheres_)
	{
		// --- ���[���h�s��̍쐬 ---
		Matrix S;
		S.MakeScaling(sphere.radius_, sphere.radius_, sphere.radius_);
		Matrix R;
		R.MakeRotation(Vector3::Zero_);
		Matrix T;
		T.MakeTranslation(sphere.position_);
		Matrix W = S * R * T;

		// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = sphere.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		// --- �`�� ---
		dc->DrawIndexed(indexCount, 0, 0);
	}
	spheres_.clear();


	// ===== �~���̕`�� =======================================================================================================================================

	// --- �v���~�e�B�u�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- �o�b�t�@��ݒ� ---
	dc->IASetVertexBuffers(0, 1, capsuleVertexBuffer_.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(capsuleIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- �`�悷��񐔂��v�Z ---
	capsuleIndexBuffer_->GetDesc(&bufferDesc);
	indexCount = bufferDesc.ByteWidth / sizeof(uint32_t);

	for (const Capsule& Capsule : capsules_)
	{
		// --- ���[���h�s��̍쐬 ---
		Matrix S;
		S.MakeScaling(Capsule.radius_, Capsule.height_, Capsule.radius_);
		Matrix R;
		R.MakeRotation(Vector3::Zero_);
		Matrix T;
		T.MakeTranslation(Capsule.position_);
		Matrix W = S * R * T;

		// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = Capsule.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		// --- �`�� ---
		dc->DrawIndexed(indexCount, 0, 0);
	}
	capsules_.clear();


	// ===== �����̕`�� ======================================================================================================================================================

	// --- �v���~�e�B�u�ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- �o�b�t�@��ݒ� ---
	dc->IASetVertexBuffers(0, 1, lineVertexBuffer_.GetAddressOf(), &stride, &offset);


	for (const Line& line : lines_)
	{
		HRESULT hr{ S_OK };
		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
		hr = dc->Map(lineVertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		Vector3* vertices{ reinterpret_cast<Vector3*>(mappedSubresource.pData) };

		vertices[0] = line.src_;
		vertices[1] = line.dst_;

		dc->Unmap(lineVertexBuffer_.Get(), 0);


		// --- ���[���h�s��̍쐬 ---
		Matrix S;
		S.MakeScaling(Vector3::Unit_);
		Matrix R;
		R.MakeRotation(Vector3::Zero_);
		Matrix T;
		T.MakeTranslation(Vector3::Zero_);
		Matrix W = S * R * T;

		// --- �I�u�W�F�N�g�萔�o�b�t�@�X�V ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = Vector4::White_;
		cb.color_ = Vector4::Gray_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		dc->Draw(2, 0);
	}
	lines_.clear();
}
