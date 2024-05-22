#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Matrix.h"


struct Texture;

class SpriteRenderer
{
public:
	// --- コンストラクタ ---
	SpriteRenderer(ID3D11Device* device);

	// --- 描画処理 ---
	void Draw(
		ID3D11DeviceContext* dc,
		ID3D11ShaderResourceView** srv,
		const float texWidth,						// 元画像の高さ
		const float texHeight,						// 元画像の幅
		const Vector3& position,					// 位置
		const Vector3& scale,						// スケール
		const Vector2& texPos,						// 切り抜き位置
		const Vector2& texSize,						// 切り抜きサイズ
		const Vector2& center = Vector2::Zero_,		// 中心
		const Vector3& rotation = Vector3::Zero_,	// 回転量
		const Vector4& color = Vector4::White_,		// 色
		bool inWorld = false,						// ワールド空間に描画するか
		bool useBillboard = false,					// ビルボードを使用するか
		ID3D11PixelShader* replacementPS = nullptr,	// 置き換えるピクセルシェーダー
		ID3D11VertexShader* replacementVS = nullptr,// 置き換える頂点シェーダー
		UINT startSlot = 0,							// テクスチャをセットする場所
		UINT numViews = 1							// セットする個数
	);


	void Draw(
		ID3D11DeviceContext* dc,
		Texture& texture,
		const Vector3& position,
		const Vector3& scale,
		const Vector2& texPos,
		const Vector2& texSize,
		const Vector2& center = Vector2::Zero_,
		const Vector3& rotation = Vector3::Zero_,
		const Vector4& color = Vector4::White_,
		bool inWorld = false,
		bool useBillboard = false,
		ID3D11PixelShader* replacementPS = nullptr,
		ID3D11VertexShader* replacementVS = nullptr,
		UINT startSlot = 0,
		UINT numViews = 1
	);


	// --- 2Dオブジェクトの定数バッファ構造体 ---
	struct ObjectConstants2D
	{
		Vector2 position_;
		Vector2 size_;
		Vector4 color_;
		Vector2 center_;
		Vector2 scale_;
		Vector2 texPos_;
		Vector2 texSize_;
		float	rotation_;
	};

	// --- 3Dオブジェクトの定数バッファ構造体 ---
	struct ObjectConstants3D
	{
		Matrix  world_;
		Vector4 color_;
		Vector2 offset_;
		Vector2 size_;
		Vector2 texPos_;
		Vector2 texSize_;
	};

	// --- 頂点データ構造体 ---
	struct Vertex
	{
		Vector3 position_;
		Vector2 texcoord_;
	};

	// --- シェーダーのリソース構造体 ---
	struct ShaderResources
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>		vertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer_;
	};

	enum
	{
		SPRITE2D,
		SPRITE3D,
	};

private:
	ShaderResources shaderResources_[2];
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;
};

