#pragma once

#include "MeshRenderer.h"


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

	// --- ���\�[�X�̎擾 ---
	const Resource& GetResource() { return resources_; }

	// --- ���f���̓ǂݍ��� ---
	MeshRenderer* LoadModel(ID3D11Device* device, const char* fileName, bool triangulate = false, bool isRayCast = false, std::string* path = nullptr);

	// --- ���f���̎擾 ---
	MeshRenderer* GetModel(const char* fileName, std::string* path = nullptr, bool triangulate = false, bool isRayCast = false);

	// --- �f�o�b�OGui�̍X�V ---
	void UpdateDebugGui();

private:
	Resource resources_;
};

