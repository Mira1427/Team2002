#include "GeometricPrimitiveBatch.h"

#include "Shader.h"


GeometricPrimitiveBatch::GeometricPrimitiveBatch(ID3D11Device* device, size_t maxInstances) : MAX_INSTANCES_(maxInstances)
{
	Vector3 vertices[] =
	{
		{ -0.5f,  0.5f, -0.5f },	// 0 : ��O����
		{  0.5f,  0.5f, -0.5f },	// 1 : ��O�E��
		{ -0.5f, -0.5f, -0.5f },	// 2 : ��O����
		{  0.5f, -0.5f, -0.5f },	// 3 : ��O�E��
		{ -0.5f,  0.5f,  0.5f },	// 4 : ������
		{  0.5f,  0.5f,  0.5f },	// 5 : ���E��
		{ -0.5f, -0.5f,  0.5f },	// 6 : ������
		{  0.5f, -0.5f,  0.5f },	// 7 : ���E��
	};

	uint32_t indices[36] =
	{
		0, 1, 2,	// ��O
		1, 2, 3,
		4, 1, 0,	// ��
		4, 5, 1,
		4, 0, 6,	// ��
		0, 2, 6,
		1, 5, 3,	// �E
		5, 3, 7,
		4, 5, 7,	// ��
		4, 7, 6,
		2, 3, 6,	// ��
		3, 6, 7

	};


	// --- �o�b�t�@�̍쐬 ---
	Shader::CreateVertexBuffer(device, vertexBuffer_.GetAddressOf(), sizeof(vertices), vertices, D3D11_USAGE_DEFAULT, 0);
	Shader::CreateIndexBuffer(device, indexBuffer_.GetAddressOf(), sizeof(indices), indices);


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


	// --- �V�F�[�_�[�̍쐬 ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"Position",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0},
		{"InstanceWorld",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceWorld",   1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceWorld",   2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceWorld",   3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceColor",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	Shader::CreateVSAndILFromCSO(
		device,
		"./Data/Shader/GeometricPrimitive_VS.cso",
		vertexShader_.GetAddressOf(),
		inputLayout_.GetAddressOf(),
		inputElementDesc,
		ARRAYSIZE(inputElementDesc)
	);

	Shader::CreatePSFromCSO(
		device,
		"./Data/Shader/GeometricPrimitive_PS.cso",
		pixelShader_.GetAddressOf()
	);
 }


// --- �`��̑O���� ---
void GeometricPrimitiveBatch::Begin(ID3D11DeviceContext* dc)
{	
	// --- �o�b�t�@�̃o�C���h ---
	UINT strides[2] = { sizeof(Vector3), sizeof(InstanceData) };
	UINT offsets[2] = { 0U, 0U };
	ID3D11Buffer* vertexBuffers[2] = { vertexBuffer_.Get(), instanceBuffer_.Get() };

	// --- �o�b�t�@�̃o�C���h ---
	dc->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
	dc->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- �V�F�[�_�[�̃o�C���h ---
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);

	// --- ���̓��C�A�E�g�̃o�C���h ---
	dc->IASetInputLayout(inputLayout_.Get());

	// --- �`��@�̐ݒ� ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	// --- �C���X�^���X�f�[�^�̃N���A ---
	instances_.clear();
}


// --- �`�揈�� ---
void GeometricPrimitiveBatch::Draw(
	ID3D11DeviceContext* dc,
	Matrix world,
	Vector4 color
)
{
	// --- �C���X�^���X�f�[�^�̒ǉ� ---
	instances_.emplace_back(world, color);
}


// --- �`��̌㏈�� ---
void GeometricPrimitiveBatch::End(ID3D11DeviceContext* dc)
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


	// --- �C���f�b�N�X�ŕ`�� ---
	D3D11_BUFFER_DESC bufferDesc{};
	indexBuffer_.Get()->GetDesc(&bufferDesc);
	dc->DrawIndexedInstanced(static_cast<UINT>(bufferDesc.ByteWidth / sizeof(uint32_t)), static_cast<UINT>(instanceCount), 0, 0, 0);
}
