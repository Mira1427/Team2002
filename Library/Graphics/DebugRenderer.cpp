#include "DebugRenderer.h"

#include <vector>

#include "../Library/Misc.h"

#include "Graphics.h"
#include "Shader.h"


// --- コンストラクタ ---
DebugRenderer::DebugRenderer(ID3D11Device* device)
{
	// --- 入力レイアウトの作成 ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// --- シェーダーの読み込み ---
	Shader::CreateVSAndILFromCSO(device, "./Data/Shader/DebugRenderer_VS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	Shader::CreateVSFromCSO(device, "./Data/Shader/DebugRenderer2D_VS.cso", vertexShader2D_.GetAddressOf());
	Shader::CreatePSFromCSO(device, "./Data/Shader/DebugRenderer_PS.cso", pixelShader_.GetAddressOf());

	Shader::CreateConstantBuffer(device, constantBuffer_.GetAddressOf(), sizeof(Constants3D));
	Shader::CreateConstantBuffer(device, constantBuffer2D_.GetAddressOf(), sizeof(Constants2D));
	
	// --- 長方形の作成 ---
	CreateRectangle(device);

	// --- 円の作成 ---
	CreateCircle(device, 18);

	// --- 立方体の作成 ---
	CreateCube(device);

	// --- 球の作成 ---
	CreateSphere(device, 18);

	// --- 円柱の作成 ---
	CreateCapsule(device, 18);

	// --- 線分の作成 ---
	CreateLine(device);
}


// ===== 長方形 ============================================================================================================================================

// --- 作成 ---
void DebugRenderer::CreateRectangle(ID3D11Device* device)
{
	// --- 頂点情報の作成 ---
	Vector3 vertices[5]
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },
	};

	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(device, rectangleVertexBuffer_.GetAddressOf(), static_cast<UINT>(sizeof(vertices)), vertices, D3D11_USAGE_DEFAULT, 0);
}

// --- 描画 ---
void DebugRenderer::DrawRectangle(const Vector2& position, const Vector2& size, const Vector2& center, float rotation, const Vector4& color)
{
	Rectangle rectangle;
	rectangle.position_ = position;
	rectangle.size_		= size;
	rectangle.center_	= center;
	rectangle.rotation_ = rotation;
	rectangle.color_	= color;
	rectangles_.emplace_back(rectangle);
}


// ===== 円 =================================================================================================================================================

// --- 作成 ---
void DebugRenderer::CreateCircle(ID3D11Device* device, int vertexNum)
{
	// --- 頂点情報の作成 ---
	std::vector<Vector3> vertices;
	for (int i = 0; i < vertexNum + 1; i++)
	{
		float radian = DirectX::XMConvertToRadians((360.0f / vertexNum) * i);
		Vector3 vertex = { cosf(radian), sinf(radian), 0.0f };
		vertices.emplace_back(vertex);
	}

	// --- 頂点の数 ---
	circleVertexCount_ = vertices.size();

	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(device, circleVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);
}

// --- 描画 ---
void DebugRenderer::DrawCircle(const Vector2& position, float radius, const Vector4& color)
{
	Circle circle;
	circle.position_ = position;
	circle.radius_ = radius;
	circle.color_ = color;
	circles_.emplace_back(circle);
}


// ===== 立方体 ============================================================================================================================================

// --- 作成 ---
void DebugRenderer::CreateCube(ID3D11Device* device)
{	
	// --- 頂点情報の作成 ---
	std::vector<Vector3> vertices =	{
		// --- 手前 ---
		{ -0.5f,  0.5f, -0.5f},	// 左上
		{  0.5f,  0.5f, -0.5f},	// 右上
		{ -0.5f, -0.5f, -0.5f},	// 左下
		{  0.5f, -0.5f, -0.5f},	// 右下
		// --- 奥 ---
		{ -0.5f,  0.5f, 0.5f},	// 左上
		{  0.5f,  0.5f, 0.5f},	// 右上
		{ -0.5f, -0.5f, 0.5f},	// 左下
		{  0.5f, -0.5f, 0.5f},	// 右下
	};

	// --- 頂点のインデックス ---
	uint32_t indices[]
	{
		0, 1, 1, 3, 3, 2, 2, 0, 0, 4, 1, 5, 3, 7, 2, 6, 4, 5, 5, 7, 7, 6, 6, 4
	};


	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};
	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(device, cubeVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	// --- インデックスバッファの作成 ---
	Shader::CreateIndexBuffer(device, cubeIndexBuffer_.GetAddressOf(), static_cast<UINT>(sizeof(indices)), indices);
}

// --- 描画 ---
void DebugRenderer::DrawCube(const Vector3& position, const Vector3& size, const Vector3& rotation, const Vector4& color)
{
	Cube cube;
	cube.position_ = position;
	cube.size_ = size;
	cube.rotation_ = rotation;
	cube.rotation_.ToRadian();
	cube.color_ = color;
	cubes_.emplace_back(cube);
}


// ===== 球 =================================================================================================================================================

// --- 作成 ---
void DebugRenderer::CreateSphere(ID3D11Device* device, int vertexNum)
{
	// --- 頂点情報の作成 ---
	std::vector<Vector3> vertices;

	vertices.emplace_back(Vector3::Up_);

	for (int i = 1; i < static_cast<int>(vertexNum * 0.5f); i++)
	{
		float theta = DirectX::XMConvertToRadians((360.0f / vertexNum) * i + 90.0f);

		for (int j = 0; j < vertexNum; j++)
		{
			float radius = 1.0f * cosf(theta);
			float y = 1.0f * sinf(theta);
			float theta2 = DirectX::XMConvertToRadians((360.0f / vertexNum) * j + 90.0f);
			Vector3 vertex = { cosf(theta2) * radius, y, sinf(theta2) * radius };
			vertices.emplace_back(vertex);
		}
	}

	vertices.emplace_back(Vector3::Down_);


	// --- インデックスの設定 ---
	std::vector<uint32_t> indices;
	int layer = static_cast<int>(vertexNum * 0.5f) - 1;
	for (int i = 0; i < layer; i++)
	{
		for (int j = 0; j < vertexNum; j++)
		{
			indices.emplace_back(vertexNum * i + j + 1);
			indices.emplace_back(((j + 2 > vertexNum) ? 1 + i * vertexNum : vertexNum * i + j + 2));
		}
	}

	for (int i = 1; i < vertexNum; i++)
	{
		indices.emplace_back(0);
		indices.emplace_back(i);

		for (int j = 0; j < layer - 1; j++)
		{
			indices.emplace_back(vertexNum * j + i);
			indices.emplace_back(vertexNum * (j + 1) + i);
		}

		indices.emplace_back(vertexNum * (layer - 1) + i);
		indices.emplace_back(vertexNum * layer + 1);
	}

	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer<Vector3>(device, sphereVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	// --- インデックスバッファの作成 ---
	Shader::CreateIndexBuffer(device, sphereIndexBuffer_.GetAddressOf(), static_cast<UINT>(indices.size() * sizeof(uint32_t)), indices.data());
}

// --- 描画 ---
void DebugRenderer::DrawSphere(const Vector3& position, float radius, const Vector4& color)
{
	Sphere sphere;
	sphere.position_ = position;
	sphere.radius_ = radius;
	sphere.color_ = color;
	spheres_.emplace_back(sphere);
}


// ===== 円柱 =============================================================================================================================

// --- 作成 ---
void DebugRenderer::CreateCapsule(ID3D11Device* device, int vertexNum)
{
	// --- 頂点情報の作成 ---
	std::vector<Vector3> vertices;
	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < vertexNum; i++)
		{
			float theta = DirectX::XMConvertToRadians((360.0f / vertexNum) * i);
			Vector3 vertex = { cosf(theta), static_cast<float>(j), sinf(theta) };
			vertices.emplace_back(vertex);
		}
	}

	// --- インデックスの設定 ---
	std::vector<uint32_t> indices;
	for(int j = 0; j < 2; j++)
	{
		for (int i = 0; i < vertexNum; i++)
		{
			indices.emplace_back(vertexNum * j + i);
			indices.emplace_back(((i + 1 > vertexNum - 1) ? vertexNum * j : vertexNum * j + i + 1));
		}
	}
	for (int i = 0; i < vertexNum; i++)
	{
		indices.emplace_back(i);
		indices.emplace_back(i + vertexNum);
	}


	HRESULT hr{ S_OK };

	D3D11_BUFFER_DESC bufferDesc{};
	D3D11_SUBRESOURCE_DATA subResourceData{};
	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(device, capsuleVertexBuffer_.GetAddressOf(), static_cast<UINT>(vertices.size() * sizeof(Vector3)), vertices.data(), D3D11_USAGE_DEFAULT, 0);

	// --- インデックスバッファの作成 ---
	Shader::CreateIndexBuffer(device, capsuleIndexBuffer_.GetAddressOf(), static_cast<UINT>(indices.size() * sizeof(uint32_t)), indices.data());
}

// --- 描画 ---
void DebugRenderer::DrawCapsule(const Vector3& position, float radius, float height, const Vector3& rotation, const Vector4& color)
{
	Capsule Capsule;
	Capsule.position_ = position;
	Capsule.radius_ = radius;
	Capsule.height_ = height;
	Capsule.rotation_ = rotation;
	Capsule.rotation_.ToRadian();
	Capsule.color_ = color;
	capsules_.emplace_back(Capsule);
}


// ===== 線分 ======================================================================================================================================================
void DebugRenderer::CreateLine(ID3D11Device* device)
{	
	// --- 頂点情報の作成 ---
	Vector3 vertices[2];

	// --- 頂点バッファの作成 ---
	Shader::CreateVertexBuffer(
		device, 
		lineVertexBuffer_.GetAddressOf(),
		static_cast<UINT>(sizeof(vertices)),
		vertices,
		D3D11_USAGE_DYNAMIC,
		D3D11_CPU_ACCESS_WRITE
	);
}

// --- 描画 ---
void DebugRenderer::DrawLine(const Vector3& src, const Vector3& dst, const Vector4& color)
{
	Line line;
	line.src_ = src;
	line.dst_ = dst;
	line.color_ = color;
	lines_.emplace_back(line);
}


// --- 描画 ---
void DebugRenderer::Draw(ID3D11DeviceContext* dc)
{
	// --- シェーダー設定 ---
	dc->VSSetShader(vertexShader2D_.Get(), nullptr, 0);	// 頂点シェーダー
	dc->PSSetShader(pixelShader_.Get(), nullptr, 0);	// ピクセルシェーダー
	dc->IASetInputLayout(inputLayout_.Get());			// 入力レイアウト

	// --- 定数バッファ設定 ---
	dc->VSSetConstantBuffers(0, 1, constantBuffer2D_.GetAddressOf());	// オブジェクト定数

	UINT indexCount{};

	// ===== 長方形の描画 ==================================================================================================================================
	
	// --- プリミティブ設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// --- バッファを設定 ---
	UINT stride{ sizeof(Vector3) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, rectangleVertexBuffer_.GetAddressOf(), &stride, &offset);

	// --- リストの分だけ描画 ---
	for (const Rectangle& rectangle : rectangles_)
	{
		// --- オブジェクト定数バッファ更新 ---
		Constants2D cb;
		cb.position_	= rectangle.position_;
		cb.size_		= rectangle.size_;
		cb.rotation_	= rectangle.rotation_;
		cb.center_		= rectangle.center_;
		cb.color_		= rectangle.color_;
		dc->UpdateSubresource(constantBuffer2D_.Get(), 0, 0, &cb, 0, 0);

		// --- 描画 ---
		dc->Draw(5, 0);
	}
	rectangles_.clear();


	// ===== 円の描画 =======================================================================================================================================

	// --- プリミティブ設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	// --- バッファを設定 ---
	dc->IASetVertexBuffers(0, 1, circleVertexBuffer_.GetAddressOf(), &stride, &offset);

	for(const Circle& circle : circles_)
	{
		// --- オブジェクト定数バッファ更新 ---
		Constants2D cb;
		cb.position_ = circle.position_;
		cb.size_ = Vector2(circle.radius_, circle.radius_);
		cb.color_ = circle.color_;
		dc->UpdateSubresource(constantBuffer2D_.Get(), 0, 0, &cb, 0, 0);

		// --- 描画 ---
		dc->Draw(static_cast<UINT>(circleVertexCount_), 0U);
	}
	circles_.clear();


	// ===== 3Dの描画設定 =======================================================================================================================================
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);					// 頂点シェーダー
	dc->VSSetConstantBuffers(0, 1, constantBuffer_.GetAddressOf());		// オブジェクト定数


	// ===== 立方体の描画 ==================================================================================================================================

	// --- プリミティブ設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- バッファを設定 ---
	dc->IASetVertexBuffers(0, 1, cubeVertexBuffer_.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(cubeIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- 描画する回数を計算 ---
	D3D11_BUFFER_DESC bufferDesc{};
	cubeIndexBuffer_->GetDesc(&bufferDesc);
	indexCount = bufferDesc.ByteWidth / sizeof(uint32_t);

	// --- リストの分だけ描画 ---
	for (const Cube& cube : cubes_)
	{
		// --- ワールド行列の作成 ---
		Matrix S;
		S.MakeScaling(cube.size_);
		Matrix R;
		R.MakeRotation(cube.rotation_);
		Matrix T;
		T.MakeTranslation(cube.position_);
		Matrix W = S * R * T;

		// --- オブジェクト定数バッファ更新 ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = cube.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		// --- 描画 ---
		dc->DrawIndexed(indexCount, 0, 0);
	}
	cubes_.clear();


	// ===== 球の描画 =======================================================================================================================================

	// --- プリミティブ設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- バッファを設定 ---
	dc->IASetVertexBuffers(0, 1, sphereVertexBuffer_.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(sphereIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- 描画する回数を計算 ---
	sphereIndexBuffer_->GetDesc(&bufferDesc);
	indexCount = bufferDesc.ByteWidth / sizeof(uint32_t);

	for (const Sphere& sphere : spheres_)
	{
		// --- ワールド行列の作成 ---
		Matrix S;
		S.MakeScaling(sphere.radius_, sphere.radius_, sphere.radius_);
		Matrix R;
		R.MakeRotation(Vector3::Zero_);
		Matrix T;
		T.MakeTranslation(sphere.position_);
		Matrix W = S * R * T;

		// --- オブジェクト定数バッファ更新 ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = sphere.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		// --- 描画 ---
		dc->DrawIndexed(indexCount, 0, 0);
	}
	spheres_.clear();


	// ===== 円柱の描画 =======================================================================================================================================

	// --- プリミティブ設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- バッファを設定 ---
	dc->IASetVertexBuffers(0, 1, capsuleVertexBuffer_.GetAddressOf(), &stride, &offset);
	dc->IASetIndexBuffer(capsuleIndexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	// --- 描画する回数を計算 ---
	capsuleIndexBuffer_->GetDesc(&bufferDesc);
	indexCount = bufferDesc.ByteWidth / sizeof(uint32_t);

	for (const Capsule& Capsule : capsules_)
	{
		// --- ワールド行列の作成 ---
		Matrix S;
		S.MakeScaling(Capsule.radius_, Capsule.height_, Capsule.radius_);
		Matrix R;
		R.MakeRotation(Vector3::Zero_);
		Matrix T;
		T.MakeTranslation(Capsule.position_);
		Matrix W = S * R * T;

		// --- オブジェクト定数バッファ更新 ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = Capsule.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		// --- 描画 ---
		dc->DrawIndexed(indexCount, 0, 0);
	}
	capsules_.clear();


	// ===== 線分の描画 ======================================================================================================================================================

	// --- プリミティブ設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// --- バッファを設定 ---
	dc->IASetVertexBuffers(0, 1, lineVertexBuffer_.GetAddressOf(), &stride, &offset);


	for (const Line& line : lines_)
	{
		HRESULT hr{ S_OK };
		D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
		hr = dc->Map(lineVertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		Vector3* vertices{ reinterpret_cast<Vector3*>(mappedSubresource.pData) };

		vertices[0] = line.src_;
		vertices[1] = line.dst_;

		dc->Unmap(lineVertexBuffer_.Get(), 0);


		// --- ワールド行列の作成 ---
		Matrix S;
		S.MakeScaling(Vector3::Unit_);
		Matrix R;
		R.MakeRotation(Vector3::Zero_);
		Matrix T;
		T.MakeTranslation(Vector3::Zero_);
		Matrix W = S * R * T;

		// --- オブジェクト定数バッファ更新 ---
		Constants3D cb;
		cb.world_ = W;
		cb.color_ = line.color_;
		dc->UpdateSubresource(constantBuffer_.Get(), 0, 0, &cb, 0, 0);

		dc->Draw(2, 0);
	}
	lines_.clear();
}
