#pragma once

#include "MeshRenderer.h"
#include "InstancedMesh.h"


class ModelManager
{
private:
	ModelManager() {}
	~ModelManager() {}

public:
	static ModelManager& Instance()
	{
		static ModelManager instance;
		return instance;
	}

	using Resource = std::unordered_map<std::string, std::shared_ptr<MeshRenderer>>;
	using InstancedMeshes = std::unordered_map<std::string, std::shared_ptr<InstancedMesh>>;

	// --- ���\�[�X�̎擾 ---
	const Resource& GetResource() { return resources_; }
	const InstancedMeshes& GetInstancedMeshes() { return instancedMeshes_; }

	// --- ���f���̓ǂݍ��� ---
	MeshRenderer* LoadModel(ID3D11Device* device, const char* fileName, bool triangulate = false, bool isRayCast = false, std::string* path = nullptr);
	InstancedMesh* LoadInstancedMesh(ID3D11Device* device, const char* fileName, size_t maxInstances, bool triangulate = false, std::string* path = nullptr);

	// --- ���f���̎擾 ---
	MeshRenderer* GetModel(const char* fileName, std::string* path = nullptr, bool triangulate = false, bool isRayCast = false);
	InstancedMesh* GetInstancedMesh(const char* fileName, size_t maxInstances, std::string* path = nullptr, bool triangulate = false);

	// --- �f�o�b�OGui�̍X�V ---
	void UpdateDebugGui();

private:
	Resource resources_;
	InstancedMeshes instancedMeshes_;
};

