#include "PrimitiveBatch.h"

#include "Shader.h"


PrimitiveBatch::PrimitiveBatch(ID3D11Device* device, size_t maxInstances) : MAX_INSTANCES_(maxInstances)
{
	// --- ���_���W�̐ݒ� ---
	Vector3 vertices[4] =
	{
		{ 0.0f, 0.0f, 0.0f },	// ����
		{ 1.0f, 0.0f, 0.0f },	// �E��
		{ 0.0f, 1.0f, 0.0f },	// ����
		{ 1.0f, 1.0f, 0.0f }	// �E��
	};


	// --- ���_�o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(
		device,
		vertexBuffer_.GetAddressOf(),
		sizeof(vertices),
		vertices,
		D3D11_USAGE_IMMUTABLE,	// GPU�ł̂ݓǂݎ��
		0
	);


	// --- �C���X�^���X�o�b�t�@�̍쐬 ---
	std::unique_ptr<InstanceData[]> instances = std::make_unique<InstanceData[]>(MAX_INSTANCES_);

	Shader::CreateVertexBuffer(
		device,
		instanceBuffer_.GetAddressOf(),
		static_cast<UINT>(sizeof(InstanceData) * MAX_INSTANCES_),
		instances.get(),
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE
	);


	// ---���̓��C�A�E�g ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "INSTANCE_POSITION",	0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_CENTER",	0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_ANGLE",		0, DXGI_FORMAT_R32_FLOAT,			1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};


	// --- ���_�V�F�[�_�[�Ɠ��̓��C�A�E�g�̍쐬 ---
	Shader::CreateVSAndILFromCSO(
		device, "./Data/Shader/PrimitiveBatch_VS.cso",
		vertexShader_.GetAddressOf(),
		inputLayout_.GetAddressOf(),
		inputElementDesc,
		ARRAYSIZE(inputElementDesc)
	);



	// --- �s�N�Z���V�F�[�_�[�̍쐬 ---
	Shader::CreatePSFromCSO(
		device,
		"./Data/Shader/PrimitiveBatch_PS.cso",
		pixelShader_.GetAddressOf()
	);
}


// --- �`��̑O���� ---
void PrimitiveBatch::Begin(ID3D11DeviceContext* dc)
{
	UINT strides[2] = { sizeof(Vector3), sizeof(InstanceData) };
	UINT offsets[2] = { 0U, 0U };
	ID3D11Buffer* vertexBuffers[2] = { vertexBuffer_.Get(), instanceBuffer_.Get() };

	dc->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);		// ���_�o�b�t�@�̃o�C���h
	dc->IASetInputLayout(inputLayout_.Get());							// ���̓��C�A�E�g�̃o�C���h
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0U);					// ���_�V�F�[�_�[�̃o�C���h
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0U);					// �s�N�Z���V�F�[�_�[�̃o�C���h
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// �`��@�̐ݒ�

	// --- �C���X�^���X�f�[�^�̃N���A ---
	instances_.clear();
}


// --- �`�揈�� ( ���ۂɂ͕`��̂��߂̎����W�� ) ---
void PrimitiveBatch::Draw(
	ID3D11DeviceContext* dc,
	Vector2 position,
	Vector2 size,
	Vector2 center,
	float angle,
	Vector4 color
)
{
	// --- �f�[�^�̕ۑ� ---
	instances_.emplace_back(position, size, color, center, angle);
}


// --- �`��̌㏈�� ---
void PrimitiveBatch::End(ID3D11DeviceContext* dc)
{
	HRESULT hr = S_OK;

	// --- �C���X�^���X�o�b�t�@�ɏ������݊J�n ---
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = dc->Map(instanceBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	size_t instanceCount = instances_.size();

	// --- �ő�C���X�^���X�����z���Ă������̌x�� ---
	_ASSERT_EXPR(instanceCount < MAX_INSTANCES_, L"Number of Instances must be less than MAX_INSTANCES.");

	InstanceData* data = reinterpret_cast<InstanceData*>(mappedBuffer.pData);

	if (data)
	{
		const InstanceData* p = instances_.data();
		::memcpy_s(data, MAX_INSTANCES_ * sizeof(InstanceData), p, instanceCount * sizeof(InstanceData));
	}


	// --- �C���X�^���X�o�b�t�@�ւ̏������ݏI�� ---
	dc->Unmap(instanceBuffer_.Get(), 0U);

	// --- �C���X�^���V���O�`�� ---
	dc->DrawInstanced(4, static_cast<UINT>(instanceCount), 0, 0);
}
