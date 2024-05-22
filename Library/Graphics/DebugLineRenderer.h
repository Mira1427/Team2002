#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Matrix.h"

// ===== デバッグライン描画クラス ========================================================================================================================
class DebugLineRenderer
{
public:
	DebugLineRenderer(ID3D11Device* device, int lineNum);

	void Draw(ID3D11DeviceContext* dc);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
	UINT vertexCount_;


	struct ObjectConstant
	{
		Matrix world_;
		Vector4 color_;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer_;
};

