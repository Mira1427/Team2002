#include "SpriteRenderer.h"

#include <Windows.h>
#include <WICTextureLoader.h>

#include "Graphics.h"
#include "Shader.h"
#include "TextureManager.h"

#include "../Library/Camera.h"


// --- コンストラクタ ---
SpriteRenderer::SpriteRenderer(ID3D11Device* device)
{
	Vertex vertices[]
	{
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
	};

	Vertex vertices3D[]
	{
		{ { -0.5f,  0.5, 0.0f }, { 0.0f, 0.0f }},
		{ {  0.5f,  0.5, 0.0f }, { 1.0f, 0.0f }},
		{ { -0.5f, -0.5, 0.0f }, { 0.0f, 1.0f }},
		{ {  0.5f, -0.5, 0.0f }, { 1.0f, 1.0f }}
	};


	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Shader::CreateVSAndILFromCSO(device, "./Data/Shader/SpriteRenderer_VS.cso", shaderResources_[SPRITE2D].vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Shader::CreateVSFromCSO(device, "./Data/Shader/SpriteRenderer3D_VS.cso", shaderResources_[SPRITE3D].vertexShader_.GetAddressOf());
	Shader::CreatePSFromCSO(device, "./Data/Shader/SpriteRenderer_PS.cso", pixelShader_.GetAddressOf());

	Shader::CreateVertexBuffer(device, shaderResources_[SPRITE2D].vertexBuffer_.GetAddressOf(), sizeof(vertices), vertices, D3D11_USAGE_DEFAULT, 0);
	Shader::CreateVertexBuffer(device, shaderResources_[SPRITE3D].vertexBuffer_.GetAddressOf(), sizeof(vertices3D), vertices3D, D3D11_USAGE_DEFAULT, 0);

	Shader::CreateConstantBuffer(device, shaderResources_[SPRITE2D].constantBuffer_.GetAddressOf(), sizeof(ObjectConstants2D));
	Shader::CreateConstantBuffer(device, shaderResources_[SPRITE3D].constantBuffer_.GetAddressOf(), sizeof(ObjectConstants3D));
}



// --- 描画処理 ---
void SpriteRenderer::Draw(
	ID3D11DeviceContext* dc,
	ID3D11ShaderResourceView** srv,
	const float texWidth,
	const float texHeight,
	const Vector3& position,
	const Vector3& scale,
	const Vector2& texPos, 
	const Vector2& texSize,
	const Vector2& center, 
	const Vector3& rotation,
	const Vector4& color,
	bool inWorld,
	bool useBillboard,
	ID3D11PixelShader* replacementPS,
	ID3D11VertexShader* replacementVS,
	UINT startSlot,
	UINT numViews
)
{	// --- シェーダーのバインド ---
	dc->PSSetShader(replacementPS ? replacementPS : pixelShader_.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout_.Get());

	// --- シェーダーリソースのバインド ---
	dc->PSSetShaderResources(startSlot, numViews, srv);
	dc->VSSetShaderResources(startSlot, numViews, srv);

	// --- 頂点バッファをバインドするための変数 ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };

	// --- ワールド空間に描画 ---
	if (inWorld)
	{
		// --- 頂点シェーダーのバインド ---
		dc->VSSetShader(shaderResources_[SPRITE3D].vertexShader_.Get(), nullptr, 0);

		// --- 頂点バッファのバインド ---
		dc->IASetVertexBuffers(0, 1, shaderResources_[SPRITE3D].vertexBuffer_.GetAddressOf(), &stride, &offset);

		// --- 定数バッファのバインド ---
		dc->VSSetConstantBuffers(0, 1, shaderResources_[SPRITE3D].constantBuffer_.GetAddressOf());

		// --- 定数バッファの更新 ---
		{
			ObjectConstants3D data;
			Matrix s;
			s.MakeScaling({ texSize.x * scale.x, texSize.y * scale.y, scale.z });
			Matrix r;
			r.MakeRotation(rotation);
			Matrix t;
			t.MakeTranslation(position);
			data.world_ = s * r * t;

			data.color_ = color;

			data.offset_ = center;

			data.size_ = texSize;
			data.texPos_ = texPos;
			data.texSize_ = { texWidth, texHeight };

			Shader::UpdateConstantBuffer(dc, shaderResources_[SPRITE3D].constantBuffer_.Get(), data);
		}
	}


	// --- スクリーン空間に描画 ---
	else
	{
		// --- 頂点シェーダーのバインド ---
		dc->VSSetShader(replacementVS ? replacementVS : shaderResources_[SPRITE2D].vertexShader_.Get(), nullptr, 0);

		// --- 頂点バッファのバインド ---
		dc->IASetVertexBuffers(0, 1, shaderResources_[SPRITE2D].vertexBuffer_.GetAddressOf(), &stride, &offset);

		// --- 定数バッファのバインド ---
		dc->VSSetConstantBuffers(0, 1, shaderResources_[SPRITE2D].constantBuffer_.GetAddressOf());

		// --- 定数バッファの更新 ---
		ObjectConstants2D data;
		data.position_ = { position.x, position.y };
		data.size_ = texSize;
		data.color_ = color;
		data.center_ = center;
		data.scale_ = { scale.x, scale.y };
		data.rotation_ = rotation.x;
		data.texPos_ = texPos;
		data.texSize_ = { texWidth, texHeight };


		if (useBillboard)
		{
			D3D11_VIEWPORT viewport;
			UINT num = 1;
			dc->RSGetViewports(&num, &viewport);

			auto& camera = Camera::Instance();

			Vector3 worldPosition;
			DirectX::XMStoreFloat3(&worldPosition.vec_,
				DirectX::XMVector3Project(
					position,
					viewport.TopLeftX, viewport.TopLeftY,
					viewport.Width, viewport.Height,
					viewport.MinDepth, viewport.MaxDepth,
					camera.GetProjection(),
					camera.GetView(),
					Matrix::Identify_
				)
			);

			data.position_ = Vector2(worldPosition.x, worldPosition.y);
		}

		Shader::UpdateConstantBuffer(dc, shaderResources_[SPRITE2D].constantBuffer_.Get(), data);
	}


	// --- 描画法設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// --- 描画 ---
	dc->Draw(4, 0);

}


void SpriteRenderer::Draw(
	ID3D11DeviceContext* dc,
	Texture& texture,
	const Vector3& position,
	const Vector3& scale,
	const Vector2& texPos,
	const Vector2& texSize,
	const Vector2& center,
	const Vector3& rotation,
	const Vector4& color,
	bool inWorld,
	bool useBillboard,
	ID3D11PixelShader* replacementPS,
	ID3D11VertexShader* replacementVS,
	UINT startSlot,
	UINT numViews
)
{
	Draw(
		dc,
		texture.srv_.GetAddressOf(),
		texture.width_,
		texture.height_,
		position,
		scale,
		texPos,
		texSize,
		center,
		rotation,
		color,
		inWorld,
		useBillboard,
		replacementPS,
		replacementVS,
		startSlot,
		numViews
	);
}