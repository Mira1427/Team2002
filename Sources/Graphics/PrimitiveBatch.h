#pragma once

#include <vector>

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Vector.h"


class PrimitiveBatch
{
public:
	PrimitiveBatch(ID3D11Device* device, size_t maxInstances);
	~PrimitiveBatch() {}

	// --- �`��̑O���� ---
	void Begin(ID3D11DeviceContext* dc);

	// --- �`�揈�� ( ���ۂɂ͕`��̂��߂̎����W�� ) ---
	void Draw(
		ID3D11DeviceContext* dc,
		Vector2 position,
		Vector2 size,
		Vector2 center,
		float angle,
		Vector4 color
	);

	// --- �`��̌㏈�� ---
	void End(ID3D11DeviceContext* dc);


private:
	struct InstanceData
	{
		InstanceData() {}

		InstanceData(const Vector2& position, const Vector2& size, const Vector4& color, const Vector2& center, float angle)
			: position_(position), size_(size), color_(color), center_(center), angle_(angle)
		{}

		Vector2 position_;
		Vector2 size_;
		Vector4 color_;
		Vector2 center_;
		float angle_;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer_;
	std::vector<InstanceData> instances_;
	const size_t MAX_INSTANCES_;
};

