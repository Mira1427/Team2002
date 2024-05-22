#pragma once

#include "MeshRenderer.h"

#include "../Math/Matrix.h"



// --- �V���A���C�Y�֌W ---
template<class T>
void serialize(T& archive, Vector2& v)
{
	archive(
		cereal::make_nvp("x", v.x),
		cereal::make_nvp("y", v.y)
	);
}

template<class T>
void serialize(T& archive, Vector3& v)
{
	archive(
		cereal::make_nvp("x", v.x),
		cereal::make_nvp("y", v.y),
		cereal::make_nvp("z", v.z)
	);
}

template<class T>
void serialize(T& archive, Vector4& v)
{
	archive(
		cereal::make_nvp("x", v.x),
		cereal::make_nvp("y", v.y),
		cereal::make_nvp("z", v.z),
		cereal::make_nvp("w", v.w)
	);
}

template<class T>
void serialize(T& archive, Matrix& m)
{
	archive(
		cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
		cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
		cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
		cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
	);
}



class InstancedMesh
{
public:
	InstancedMesh(ID3D11Device* device, const char* fileName, bool triangulate, size_t maxInstances);
	virtual ~InstancedMesh() = default;


	// --- �`��̑O���� ---
	void Begin(ID3D11DeviceContext* dc);


	// --- �`�揈�� ---
	void Draw(
		ID3D11DeviceContext* dc,
		const Matrix& world,
		const Vector4& color
	);


	// --- �`��̌㏈�� ---
	void End(ID3D11DeviceContext* dc);



	// --- ���_�f�[�^ ---
	struct Vertex
	{
		Vector3 position_;
		Vector3 normal_ = { 0.0f, 1.0f, 0.0f };
		Vector4 tangent_ = { 1.0f, 0.0f, 0.0f, 1.0f };
		Vector2 texCoord_;

		template<class T>
		void serialize(T& archive)
		{
			archive(position_, normal_, tangent_, texCoord_);
		}
	};

	// --- ���b�V�� ---
	struct Mesh
	{
		uint64_t uniqueID_ = { 0 };
		std::string name_;
		int64_t nodeIndex_ = { 0 };

		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;

		Matrix defaultGlobalTransform_
		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};


		struct Subset
		{
			uint64_t materialUniqueID_ = 0;
			std::string materialName_;

			uint32_t startIndexLocation_ = 0;
			uint32_t indexCount_ = 0;


			template<class T>
			void serialize(T& archive)
			{
				archive(
					materialUniqueID_,
					materialName_,
					startIndexLocation_,
					indexCount_
				);
			}
		};

		std::vector<Subset> subsets_;


		template<class T>
		void serialize(T& archive)
		{
			archive(
				uniqueID_,
				name_,
				nodeIndex_,
				subsets_,
				defaultGlobalTransform_,
				vertices_,
				indices_
			);
		}


	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer_;
		friend class InstancedMesh;
	};


	using Material = std::unordered_map<uint64_t, MeshRenderer::Material>;
	
	std::vector<Mesh> meshes_;	// ���b�V��
	Material materials_;		// �}�e���A��
	NodeTree sceneView_;		// �m�[�h


	// --- �C���X�^���X�̃f�[�^ ---
	struct InstanceData
	{
		InstanceData() {}
		InstanceData(const Matrix& world, const Vector4& color) : world_(world), color_(color) {}

		Matrix  world_;
		Vector4 color_;
	};

	const size_t MAX_INSTANCES_;

	std::vector<InstanceData> instances_;


private:
	// --- �t�@�C�����烁�b�V�����擾 ---
	void FetchMeshes(
		fbxsdk::FbxScene* fbxScene,
		std::vector<Mesh>& meshes
	);


	// --- �t�@�C������}�e���A�����擾 ---
	void FetchMaterials(
		fbxsdk::FbxScene* fbxScene,
		std::unordered_map<uint64_t, MeshRenderer::Material>& materials
	);


	// --- COM�I�u�W�F�N�g�̍쐬 ---
	void CreateCOMObject(
		ID3D11Device* device,
		const char* fileName
	);


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout_;
};

