#include "DebugLineRenderer.h"

#include <vector>

#include "Shader.h"

// ===== デバッグライン描画クラス ========================================================================================================================
DebugLineRenderer::DebugLineRenderer(ID3D11Device* device, int lineNum)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	Shader::CreateVSAndILFromCSO(device, "./Data/Shader/DebugRenderer_VS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Shader::CreatePSFromCSO(device, "./Data/Shader/DebugRenderer_PS.cso", pixelShader_.GetAddressOf());

	std::vector<Vector3> vertices;

	for (int i = -(lineNum / 2); i <= (lineNum / 2); i++)
	{
		vertices.emplace_back(Vector3(static_cast<float>(i), 0, lineNum / 2.0f));
		vertices.emplace_back(Vector3(static_cast<float>(i), 0, -(lineNum / 2.0f)));
		vertices.emplace_back(Vector3(lineNum / 2.0f, 0, static_cast<float>(i)));
		vertices.emplace_back(Vector3(-(lineNum / 2.0f), 0, static_cast<float>(i)));
	}

	vertexCount_ = static_cast<UINT>(vertices.size());

	Shader::CreateVertexBuffer(device, vertexBuffer_.GetAddressOf(), static_cast<UINT>(vertexCount_ * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	Shader::CreateConstantBuffer(device, constantBuffer_.GetAddressOf(), sizeof(ObjectConstant));
}


void DebugLineRenderer::Draw(ID3D11DeviceContext* dc)
{
	// --- シェーダー設定 ---
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout_.Get());

	// --- 頂点バッファ設定 ---
	UINT stride{ sizeof(Vector3) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);

	// --- 定数バッファ設定 ---
	dc->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());

	// --- 描画法設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- ワールド行列の作成 ---
	Matrix S;
	S.MakeScaling(Vector3::Unit_);
	Matrix R;
	R.MakeRotation(Vector3::Zero_);
	Matrix T;
	T.MakeTranslation(Vector3::Zero_);
	Matrix W = S * R * T;

	// --- オブジェクト定数バッファ更新 ---
	ObjectConstant data;
	data.world_ = W.mat_;
	data.color_ = Vector4::Gray_;
	Shader::UpdateConstantBuffer(dc, constantBuffer_.Get(), data);

	dc->Draw(vertexCount_, 0);
}
