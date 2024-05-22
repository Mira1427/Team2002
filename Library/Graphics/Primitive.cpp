#include "Primitive.h"

#include "Graphics.h"
#include "Shader.h"


static void rotate(float& x, float& y, float cx, float cy, float angle);


// --- コンストラクタ ---
Primitive::Primitive(ID3D11Device* device)
{	
	HRESULT hr = S_OK;

	// --- 頂点情報のセット ---
	Vertex vertices[] = {

		{ { -1.0, +1.0, 0 }, {1.0f, 1.0f, 1.0f, 1.0f} },
		{ { +1.0, +1.0, 0 }, {1.0f, 1.0f, 1.0f, 1.0f} },
		{ { -1.0, -1.0, 0 }, {1.0f, 1.0f, 1.0f, 1.0f} },
		{ { +1.0, -1.0, 0 }, {1.0f, 1.0f, 1.0f, 1.0f} }
	};


	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(
		device,
		vertexBuffer_.GetAddressOf(),
		sizeof(vertices),
		vertices,
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE
	);


	// --- 入力レイアウトオブジェクトの生成 ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	//	頂点シェーダーオブジェクトの生成
	Shader::CreateVSAndILFromCSO(
		device,
		"./Data/Shader/Primitive_VS.cso",
		vertexShader_.GetAddressOf(),
		inputLayout_.GetAddressOf(),
		inputElementDesc,
		ARRAYSIZE(inputElementDesc)
	);

	//	ピクセルシェーダーオブジェクトの生成
	Shader::CreatePSFromCSO(
		device,
		"./Data/Shader/Primitive_PS.cso",
		pixelShader_.GetAddressOf()
	);
}


// --- 描画処理 ---
void Primitive::Draw(ID3D11DeviceContext* dc, const Vector2& position, const Vector2& size, const Vector2& center, float angle, const Vector4& color)
{	//	スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT numViewport{ 1 };
	dc->RSGetViewports(&numViewport, &viewport);


	//	renderメンバ関数の引数から、矩形の各頂点の位置を計算する
	//	(x0, y0) *----* (x1, y1)
	//			 |   /|
	//			 |  / |
	//			 | /  |
	//			 |/   |
	//	(x2, y2) *----* (x3, y3)

	//	Left_Top
	float x0{ position.x - center.x };
	float y0{ position.y - center.y };

	//	Right_Top
	float x1{ position.x + size.x - center.x };
	float y1{ position.y - center.y };

	//	Left_Bottom
	float x2{ position.x - center.x };
	float y2{ position.y + size.y - center.y };

	//	RIght_Bottom
	float x3{ position.x + size.x - center.x };
	float y3{ position.y + size.y - center.y };


	//	回転の中心を矩形の中心点にした場合
	float cx = position.x - center.x + size.x * 0.5f;
	float cy = position.y - center.y + size.y * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);


	//	スクリーン座標系からNDCへの変換を行う
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;

	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;

	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;

	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//	計算結果で頂点バッファオブジェクトを更新する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = dc->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };

	if (vertices != nullptr) {

		vertices[0].position_ = { x0, y0, 0 };
		vertices[1].position_ = { x1, y1, 0 };
		vertices[2].position_ = { x2, y2, 0 };
		vertices[3].position_ = { x3, y3, 0 };

		vertices[0].color_ = vertices[1].color_ = vertices[2].color_ = vertices[3].color_ = color;
	}

	dc->Unmap(vertexBuffer_.Get(), 0);

	//	頂点バッファーのバインド
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);


	//	プリミティブタイプおよびデータの順序に関する情報のバインド
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	//	入力レイアウトオブジェクトのバインド
	dc->IASetInputLayout(inputLayout_.Get());


	//	シェーダーのバインド
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);


	//	プリミティブの描画
	dc->Draw(4, 0);

}


void rotate(float& x, float& y, float cx, float cy, float angle)
{
	x -= cx;
	y -= cy;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};