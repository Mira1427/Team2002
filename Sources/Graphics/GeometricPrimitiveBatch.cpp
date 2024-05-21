#include "GeometricPrimitiveBatch.h"

#include "Shader.h"


GeometricPrimitiveBatch::GeometricPrimitiveBatch(ID3D11Device* device, size_t maxInstances) : MAX_INSTANCES_(maxInstances)
{
	Vector3 vertices[] =
	{
		{ -0.5f,  0.5f, -0.5f },	// 0 : 手前左上
		{  0.5f,  0.5f, -0.5f },	// 1 : 手前右上
		{ -0.5f, -0.5f, -0.5f },	// 2 : 手前左下
		{  0.5f, -0.5f, -0.5f },	// 3 : 手前右下
		{ -0.5f,  0.5f,  0.5f },	// 4 : 奥左上
		{  0.5f,  0.5f,  0.5f },	// 5 : 奥右上
		{ -0.5f, -0.5f,  0.5f },	// 6 : 奥左下
		{  0.5f, -0.5f,  0.5f },	// 7 : 奥右下
	};

	uint32_t indices[36] =
	{
		0, 1, 2,	// 手前
		1, 2, 3,
		4, 1, 0,	// 上
		4, 5, 1,
		4, 0, 6,	// 左
		0, 2, 6,
		1, 5, 3,	// 右
		5, 3, 7,
		4, 5, 7,	// 奥
		4, 7, 6,
		2, 3, 6,	// 下
		3, 6, 7

	};


	// --- バッファの作成 ---
	Shader::CreateVertexBuffer(device, vertexBuffer_.GetAddressOf(), sizeof(vertices), vertices, D3D11_USAGE_DEFAULT, 0);
	Shader::CreateIndexBuffer(device, indexBuffer_.GetAddressOf(), sizeof(indices), indices);


	// --- インスタンスバッファの作成 ---
	std::unique_ptr<InstanceData[]> instances = std::make_unique<InstanceData[]>(MAX_INSTANCES_);

	Shader::CreateVertexBuffer(
		device,
		instanceBuffer_.GetAddressOf(),
		static_cast<UINT>(sizeof(InstanceData) * MAX_INSTANCES_),
		instances.get(),
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE
	);


	// --- シェーダーの作成 ---
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


// --- 描画の前処理 ---
void GeometricPrimitiveBatch::Begin(ID3D11DeviceContext* dc)
{	
	// --- バッファのバインド ---
	UINT strides[2] = { sizeof(Vector3), sizeof(InstanceData) };
	UINT offsets[2] = { 0U, 0U };
	ID3D11Buffer* vertexBuffers[2] = { vertexBuffer_.Get(), instanceBuffer_.Get() };

	// --- バッファのバインド ---
	dc->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
	dc->IASetIndexBuffer(indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- シェーダーのバインド ---
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);

	// --- 入力レイアウトのバインド ---
	dc->IASetInputLayout(inputLayout_.Get());

	// --- 描画法の設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	// --- インスタンスデータのクリア ---
	instances_.clear();
}


// --- 描画処理 ---
void GeometricPrimitiveBatch::Draw(
	ID3D11DeviceContext* dc,
	Matrix world,
	Vector4 color
)
{
	// --- インスタンスデータの追加 ---
	instances_.emplace_back(world, color);
}


// --- 描画の後処理 ---
void GeometricPrimitiveBatch::End(ID3D11DeviceContext* dc)
{
	HRESULT hr = S_OK;

	// --- インスタンスバッファに書き込み開始 ---
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	hr = dc->Map(instanceBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	size_t instanceCount = instances_.size();

	// --- 最大インスタンス数を越えていた時の警告 ---
	_ASSERT_EXPR(instanceCount < MAX_INSTANCES_, L"Number of Instances must be less than MAX_INSTANCES.");

	InstanceData* data = reinterpret_cast<InstanceData*>(mappedBuffer.pData);

	if (data)
	{
		const InstanceData* p = instances_.data();
		::memcpy_s(data, MAX_INSTANCES_ * sizeof(InstanceData), p, instanceCount * sizeof(InstanceData));
	}


	// --- インスタンスバッファへの書き込み終了 ---
	dc->Unmap(instanceBuffer_.Get(), 0U);


	// --- インデックスで描画 ---
	D3D11_BUFFER_DESC bufferDesc{};
	indexBuffer_.Get()->GetDesc(&bufferDesc);
	dc->DrawIndexedInstanced(static_cast<UINT>(bufferDesc.ByteWidth / sizeof(uint32_t)), static_cast<UINT>(instanceCount), 0, 0, 0);
}
