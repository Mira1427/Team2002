#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Matrix.h"


class SkyMap
{
public:
	SkyMap(ID3D11Device* device, const wchar_t* fileName);
	~SkyMap() {};

	void Draw(ID3D11DeviceContext* dc);	// 描画処理
	void OpenTexture();					// テクスチャの読み込み

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			skyMapPS_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			skyBoxPS_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;

	bool isCubeTexture_;
};

