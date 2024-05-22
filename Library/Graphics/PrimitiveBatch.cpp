#include "PrimitiveBatch.h"

#include "Shader.h"


PrimitiveBatch::PrimitiveBatch(ID3D11Device* device, size_t maxInstances) : MAX_INSTANCES_(maxInstances)
{
	// --- 頂点座標の設定 ---
	Vector3 vertices[4] =
	{
		{ 0.0f, 0.0f, 0.0f },	// 左上
		{ 1.0f, 0.0f, 0.0f },	// 右上
		{ 0.0f, 1.0f, 0.0f },	// 左下
		{ 1.0f, 1.0f, 0.0f }	// 右下
	};


	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(
		device,
		vertexBuffer_.GetAddressOf(),
		sizeof(vertices),
		vertices,
		D3D11_USAGE_IMMUTABLE,	// GPUでのみ読み取り
		0
	);


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


	// ---入力レイアウト ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,	0 },
		{ "INSTANCE_POSITION",	0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_CENTER",	0, DXGI_FORMAT_R32G32_FLOAT,		1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "INSTANCE_ANGLE",		0, DXGI_FORMAT_R32_FLOAT,			1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};


	// --- 頂点シェーダーと入力レイアウトの作成 ---
	Shader::CreateVSAndILFromCSO(
		device, "./Data/Shader/PrimitiveBatch_VS.cso",
		vertexShader_.GetAddressOf(),
		inputLayout_.GetAddressOf(),
		inputElementDesc,
		ARRAYSIZE(inputElementDesc)
	);



	// --- ピクセルシェーダーの作成 ---
	Shader::CreatePSFromCSO(
		device,
		"./Data/Shader/PrimitiveBatch_PS.cso",
		pixelShader_.GetAddressOf()
	);
}


// --- 描画の前処理 ---
void PrimitiveBatch::Begin(ID3D11DeviceContext* dc)
{
	UINT strides[2] = { sizeof(Vector3), sizeof(InstanceData) };
	UINT offsets[2] = { 0U, 0U };
	ID3D11Buffer* vertexBuffers[2] = { vertexBuffer_.Get(), instanceBuffer_.Get() };

	dc->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);		// 頂点バッファのバインド
	dc->IASetInputLayout(inputLayout_.Get());							// 入力レイアウトのバインド
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0U);					// 頂点シェーダーのバインド
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0U);					// ピクセルシェーダーのバインド
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	// 描画法の設定

	// --- インスタンスデータのクリア ---
	instances_.clear();
}


// --- 描画処理 ( 実際には描画のための資料集め ) ---
void PrimitiveBatch::Draw(
	ID3D11DeviceContext* dc,
	Vector2 position,
	Vector2 size,
	Vector2 center,
	float angle,
	Vector4 color
)
{
	// --- データの保存 ---
	instances_.emplace_back(position, size, color, center, angle);
}


// --- 描画の後処理 ---
void PrimitiveBatch::End(ID3D11DeviceContext* dc)
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

	// --- インスタンシング描画 ---
	dc->DrawInstanced(4, static_cast<UINT>(instanceCount), 0, 0);
}
