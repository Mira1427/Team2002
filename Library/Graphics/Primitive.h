#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Vector.h"


class Primitive
{
public:
	Primitive(ID3D11Device* device);
	~Primitive() {}

	// --- ï`âÊèàóù ---
	void Draw(
		ID3D11DeviceContext* dc,
		const Vector2& position,
		const Vector2& size,
		const Vector2& center,
		float angle,
		const Vector4& color
	);


private:
	struct Vertex {

		Vector3 position_;
		Vector4 color_;
	};

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
};

