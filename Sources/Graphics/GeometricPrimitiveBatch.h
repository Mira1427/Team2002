#pragma once

#include <vector>

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Matrix.h"


class GeometricPrimitiveBatch
{
public:
	GeometricPrimitiveBatch(ID3D11Device* device, size_t maxInstances);
	~GeometricPrimitiveBatch() {}

	// --- �`��̑O���� ---
	void Begin(ID3D11DeviceContext* dc);

	// --- �`�揈�� ---
	void Draw(ID3D11DeviceContext* dc, Matrix world, Vector4 color);

	// --- �`��̌㏈�� ---
	void End(ID3D11DeviceContext* dc);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		indexBuffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		instanceBuffer_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;

	struct InstanceData
	{
		InstanceData() {};
		InstanceData(const Matrix& mat, const Vector4& color) : world_(mat), color_(color) {}

		Matrix  world_;
		Vector4 color_;
	};

	std::vector<InstanceData> instances_;
	const size_t MAX_INSTANCES_;
};

