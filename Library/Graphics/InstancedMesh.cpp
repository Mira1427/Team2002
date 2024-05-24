#include "InstancedMesh.h"

#include <filesystem>
#include <fstream>

#include "Shader.h"

#include "../Library/Misc.h"


// --- 変換関数 ---
inline Matrix	 ToMatrix(const fbxsdk::FbxAMatrix& fbxAMatrix);
inline Vector3   ToVector3(const fbxsdk::FbxDouble3& fbxDouble3);
inline Vector4   ToVector4(const fbxsdk::FbxDouble4& fbxDouble4);



// --- コンストラクタ ---
InstancedMesh::InstancedMesh(ID3D11Device* device, const char* fileName, bool triangulate, size_t maxInstances) : MAX_INSTANCES_(maxInstances)
{
	std::filesystem::path cerealFileName(fileName);
	cerealFileName.replace_extension("cereal");

	//	ファイルがあったら
	if (std::filesystem::exists(cerealFileName.c_str()))
	{
		//	デシリアライズ(復元)
		std::ifstream ifs(cerealFileName.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(sceneView_, meshes_, materials_);
	}

	//	なかったらFBXをロード
	else {

		fbxsdk::FbxManager* fbxManager = fbxsdk::FbxManager::Create();
		fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(fbxManager, "");

		fbxsdk::FbxImporter* fbxImporter = fbxsdk::FbxImporter::Create(fbxManager, "");
		bool importStatus = false;
		importStatus = fbxImporter->Initialize(fileName);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		importStatus = fbxImporter->Import(fbxScene);
		_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

		fbxsdk::FbxGeometryConverter fbxConverter(fbxManager);
		if (triangulate)
		{
			fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
			fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
		}


		std::function<void(fbxsdk::FbxNode*)> traverse = [&](fbxsdk::FbxNode* fbxNode)
			{
				NodeTree::Node& node = sceneView_.nodes.emplace_back();
				node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : fbxsdk::FbxNodeAttribute::EType::eUnknown;
				node.name = fbxNode->GetName();
				node.uniqueID = fbxNode->GetUniqueID();
				node.parentIndex = sceneView_.indexOf(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);

				for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); childIndex++)
				{
					traverse(fbxNode->GetChild(childIndex));
				}
			};

		traverse(fbxScene->GetRootNode());

		FetchMeshes(fbxScene, meshes_);
		FetchMaterials(fbxScene, materials_);

		fbxManager->Destroy();


		std::ofstream ofs(cerealFileName.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive serialization(ofs);
		serialization(sceneView_, meshes_, materials_);
	}

	CreateCOMObject(device, fileName);
}


// --- 描画処理 ---
void InstancedMesh::AddDrawList(const Matrix& world, const Vector4& color)
{
	// --- インスタンスデータの追加 ---
	instances_.emplace_back(world, color);
}


// --- 描画の後処理 ---
void InstancedMesh::Draw(
	ID3D11DeviceContext* dc,
	ID3D11PixelShader** replacementPixelShader,
	bool eraserDrawList
)
{
	HRESULT hr = S_OK;

	// --- 描画法の設定 ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// --- 入力レイアウトの設定 ---
	dc->IASetInputLayout(inputLayout_.Get());

	// --- シェーダーのバインド ---
	dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
	dc->PSSetShader(replacementPixelShader ? *replacementPixelShader : pixelShader_.Get(), nullptr, 0);

	size_t instanceCount = instances_.size();

	for (const Mesh& mesh : meshes_)
	{
		// --- インスタンスバッファに書き込み開始 ---
		D3D11_MAPPED_SUBRESOURCE mappedBuffer;
		hr = dc->Map(mesh.instanceBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		// --- 最大インスタンス数を越えていた時の警告 ---
		_ASSERT_EXPR(instanceCount < MAX_INSTANCES_, L"Number of Instances must be less than MAX_INSTANCES.");

		InstanceData* data = reinterpret_cast<InstanceData*>(mappedBuffer.pData);

		if (data)
		{
			std::vector<InstanceData> instances = instances_;

			for (auto& it : instances)
				it.world_ = mesh.defaultGlobalTransform_ * it.world_;

			const InstanceData* p = instances.data();

			::memcpy_s(data, MAX_INSTANCES_ * sizeof(InstanceData), p, instanceCount * sizeof(InstanceData));
		}

		// --- インスタンスバッファへの書き込み終了 ---
		dc->Unmap(mesh.instanceBuffer_.Get(), 0U);


		uint32_t strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
		uint32_t offsets[2] = { 0, 0 };
		ID3D11Buffer* vertexBuffers[2] = { mesh.vertexBuffer_.Get(), mesh.instanceBuffer_.Get() };

		// --- バッファのバインド ---
		dc->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
		dc->IASetIndexBuffer(mesh.indexBuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);


		for (const Mesh::Subset& subset : mesh.subsets_)
		{
			const MeshRenderer::Material& material = materials_.at(subset.materialUniqueID_);

			//Vector4 col = { color.x * material.Kd.x, color.y * material.Kd.y, color.z * material.Kd.z, color.w * material.Kd.w };

			dc->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			dc->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
			dc->PSSetShaderResources(2, 1, material.shaderResourceViews[2].GetAddressOf());

			// --- インデックスで描画 ---
			D3D11_BUFFER_DESC bufferDesc{};
			mesh.indexBuffer_.Get()->GetDesc(&bufferDesc);
			dc->DrawIndexedInstanced(
				subset.indexCount_,
				static_cast<UINT>(instanceCount),
				subset.startIndexLocation_,
				0,
				0
			);
		}
	}

	// --- インスタンスのクリア ---
	if (eraserDrawList)
		instances_.clear();
}


// --- メッシュの取得 ---
void InstancedMesh::FetchMeshes(fbxsdk::FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
	for (const NodeTree::Node& node : sceneView_.nodes)
	{
		if (node.attribute != fbxsdk::FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

#if 0
		// --- 名前から取得 ---
		fbxsdk::FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
#else
		// --- ユニークIDから取得 ---
		fbxsdk::FbxNode* fbxNode = findByUniqueID(fbxScene, node.uniqueID);
#endif

		fbxsdk::FbxMesh* fbxMesh = fbxNode->GetMesh();

		if (!fbxMesh)
			continue;

		Mesh& mesh = meshes.emplace_back();

		mesh.uniqueID_ = fbxNode->GetUniqueID();
		mesh.name_ = fbxNode->GetName();
		mesh.nodeIndex_ = sceneView_.indexOf(node.uniqueID);
		mesh.defaultGlobalTransform_ = ToMatrix(fbxNode->EvaluateGlobalTransform());


		std::vector<Mesh::Subset>& subsets = mesh.subsets_;
		const int  materialCount = fbxMesh->GetNode()->GetMaterialCount();
		subsets.resize(materialCount > 0 ? materialCount : 1);

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			const fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			subsets.at(materialIndex).materialName_ = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueID_ = fbxMaterial->GetUniqueID();
		}

		if (materialCount > 0)
		{
			const int polygonCount = fbxMesh->GetPolygonCount();

			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				subsets.at(materialIndex).indexCount_ += 3;
			}

			uint32_t offset = 0;

			for (Mesh::Subset& subset : subsets)
			{
				subset.startIndexLocation_ = offset;
				offset += subset.indexCount_;
				subset.indexCount_ = 0;
			}
		}

		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices_.resize(polygonCount * 3LL);
		mesh.indices_.resize(polygonCount * 3LL);

		fbxsdk::FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);
		const fbxsdk::FbxVector4* controlPoints = fbxMesh->GetControlPoints();

		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			const int materialIndex = (materialCount > 0) ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
			Mesh::Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndexLocation_ + subset.indexCount_;

			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
			{
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;

				Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);
				vertex.position_.x = static_cast<float>(controlPoints[polygonVertex][0]);
				vertex.position_.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position_.z = static_cast<float>(controlPoints[polygonVertex][2]);



				if (fbxMesh->GetElementNormalCount() > 0)
				{
					fbxsdk::FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal_.x = static_cast<float>(normal[0]);
					vertex.normal_.y = static_cast<float>(normal[1]);
					vertex.normal_.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0)
				{
					fbxsdk::FbxVector2 uv;
					bool unmappedUV;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUV);
					vertex.texCoord_.x = static_cast<float>(uv[0]);
					vertex.texCoord_.y = 1.0f - static_cast<float>(uv[1]);
				}


				//	法線ベクトルの値を取得
				if (fbxMesh->GenerateTangentsData(0, false))
				{
					const fbxsdk::FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent_.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent_.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent_.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent_.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}

				mesh.vertices_.at(vertexIndex) = std::move(vertex);
				mesh.indices_.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount_++;
			}
		}
	}

}


// --- マテリアルの取得 ---
void InstancedMesh::FetchMaterials(fbxsdk::FbxScene* fbxScene, std::unordered_map<uint64_t, MeshRenderer::Material>& materials)
{
	const size_t nodeCount = sceneView_.nodes.size();

	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		const NodeTree::Node& node = sceneView_.nodes.at(nodeIndex);
#if 0
		const fbxsdk::FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
#else
		const fbxsdk::FbxNode* fbxNode = findByUniqueID(fbxScene, node.uniqueID);
#endif

		const int materialCount = fbxNode->GetMaterialCount();

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			const fbxsdk::FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);

			MeshRenderer::Material material;
			material.name.assign(fbxMaterial->GetName(), fbxMaterial->GetName() + strlen(fbxMaterial->GetName()));
			material.uniqueID = fbxMaterial->GetUniqueID();
			fbxsdk::FbxProperty fbxProperty;

			fbxProperty = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);

			//	Kd
			if (fbxProperty.IsValid())
			{
				const fbxsdk::FbxDouble3 color = fbxProperty.Get<fbxsdk::FbxDouble3>();
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				const fbxsdk::FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<fbxsdk::FbxFileTexture>();

				material.textureFileNames[0] = ((fbxTexture) ? fbxTexture->GetRelativeFileName() : "");
			}


			//	Ka
			fbxProperty = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sAmbient);

			if (fbxProperty.IsValid())
			{
				const fbxsdk::FbxDouble3 color = fbxProperty.Get<fbxsdk::FbxDouble3>();
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;
			}


			//	Ks
			fbxProperty = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sSpecular);

			if (fbxProperty.IsValid())
			{
				const fbxsdk::FbxDouble3 color = fbxProperty.Get<fbxsdk::FbxDouble3>();
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;
			}


			fbxProperty = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sNormalMap);

			//	法線マップのファイル名を取得
			if (fbxProperty.IsValid())
			{
				const fbxsdk::FbxFileTexture* fileTexture = fbxProperty.GetSrcObject<fbxsdk::FbxFileTexture>();

				material.textureFileNames[1] = ((fileTexture) ? fileTexture->GetRelativeFileName() : "");
			}


			fbxProperty = fbxMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sEmissive);

			// --- エミッシブのファイル名を取得 ---
			if (fbxProperty.IsValid())
			{
				const fbxsdk::FbxFileTexture* fileTexture = fbxProperty.GetSrcObject<fbxsdk::FbxFileTexture>();

				material.textureFileNames[2] = ((fileTexture) ? fileTexture->GetRelativeFileName() : "");
			}

			materials.emplace(material.uniqueID, std::move(material));
		}
	}

}


// --- COMオブジェクトの作成 ---
void InstancedMesh::CreateCOMObject(ID3D11Device* device, const char* fileName)
{
	HRESULT hr = S_OK;

	for (Mesh& mesh : meshes_)
	{
		// --- 頂点バッファの作成 ---
		Shader::CreateVertexBuffer(
			device,
			mesh.vertexBuffer_.GetAddressOf(),
			static_cast<UINT>(sizeof(Vertex) * mesh.vertices_.size()),
			mesh.vertices_.data(),
			D3D11_USAGE_DEFAULT,
			0
		);


		// --- インデックスバッファの作成 ---
		Shader::CreateIndexBuffer(
			device,
			mesh.indexBuffer_.GetAddressOf(),
			static_cast<UINT>(sizeof(uint32_t) * mesh.indices_.size()),
			mesh.indices_.data()
		);


		// --- インスタンスバッファの作成 ---
		std::unique_ptr<InstanceData[]> instances = std::make_unique<InstanceData[]>(MAX_INSTANCES_);

		Shader::CreateVertexBuffer(
			device,
			mesh.instanceBuffer_.GetAddressOf(),
			static_cast<UINT>(sizeof(InstanceData) * MAX_INSTANCES_),
			instances.get(),
			D3D11_USAGE_DYNAMIC,
			D3D11_CPU_ACCESS_WRITE
		);


#if 1
		mesh.vertices_.clear();
		mesh.indices_.clear();
#endif
	}


	// シェーダーリソースビューオブジェクトの生成
	for (std::unordered_map<uint64_t, MeshRenderer::Material>::iterator iterator = materials_.begin(); iterator != materials_.end(); ++iterator)
	{
		for (size_t textureIndex = 0; textureIndex < static_cast<size_t>(MaterialLabel::MAX); textureIndex++)
		{
			if (iterator->second.textureFileNames[textureIndex].size() > 0) {

				std::filesystem::path path(fileName);
				path.replace_filename(iterator->second.textureFileNames[textureIndex]);
				D3D11_TEXTURE2D_DESC texture2dDesc{};
				Shader::LoadTextureFromFile(device, path.c_str(), iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
			}

			else
			{
				DWORD value = {};

				switch (textureIndex)
				{
				case static_cast<size_t>(MaterialLabel::DIFFUSE):			value = 0xFFFFFFFF; break;
				case static_cast<size_t>(MaterialLabel::NORMAL):			value = 0xFFFF7F7F; break;
				case static_cast<size_t>(MaterialLabel::EMISSIVE):			value = 0xFF000000; break;
				case static_cast<size_t>(MaterialLabel::ROUGHNESS):			value = 0xFF000000; break;
				case static_cast<size_t>(MaterialLabel::METARIC):			value = 0xFF000000; break;
				case static_cast<size_t>(MaterialLabel::AMBIENT_OCCLUSION): value = 0xFFFFFFFF; break;
				}

				Shader::MakeDummyTexture(device, iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), value, 16);
			}
		}
	}


	// ---　入力レイアウト ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0},
		{"NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0},
		{"TANGENT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0},
		{"TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0},
		{"InstanceWorld",   0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceWorld",   1, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceWorld",   2, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceWorld",   3, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{"InstanceColor",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};


	// --- 頂点シェーダーと入力レイアウトの作成 ---
	Shader::CreateVSAndILFromCSO(
		device,
		"./Data/Shader/InstancedMesh_VS.cso",
		vertexShader_.ReleaseAndGetAddressOf(),
		inputLayout_.ReleaseAndGetAddressOf(),
		inputElementDesc,
		ARRAYSIZE(inputElementDesc)
	);


	// --- ピクセルシェーダーの作成 ---
	Shader::CreatePSFromCSO(
		device,
		"./Data/Shader/PBR.cso",
		//"./Data/Shader/InstancedMesh_PS.cso",
		pixelShader_.ReleaseAndGetAddressOf()
	);

}



// --- 変換関数 ---
inline Matrix ToMatrix(const fbxsdk::FbxAMatrix& fbxAMatrix) {

	Matrix matrix;

	for (int row = 0; row < 4; row++) {

		for (int column = 0; column < 4; column++) {

			matrix.m_[row][column] = static_cast<float>(fbxAMatrix[row][column]);
		}
	}

	return matrix;
}


inline Vector3 ToVector3(const fbxsdk::FbxDouble3& fbxDouble3) {

	Vector3 vec3;

	vec3.x = static_cast<float>(fbxDouble3[0]);
	vec3.y = static_cast<float>(fbxDouble3[1]);
	vec3.z = static_cast<float>(fbxDouble3[2]);

	return vec3;
}


inline Vector4 ToVector4(const fbxsdk::FbxDouble4& fbxDouble4) {

	Vector4 vec4;

	vec4.x = static_cast<float>(fbxDouble4[0]);
	vec4.y = static_cast<float>(fbxDouble4[1]);
	vec4.z = static_cast<float>(fbxDouble4[2]);
	vec4.w = static_cast<float>(fbxDouble4[3]);

	return vec4;
}