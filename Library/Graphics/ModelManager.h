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

	// --- リソースの取得 ---
	const Resource& GetResource() { return resources_; }

	// --- モデルの読み込み ---
	MeshRenderer* LoadModel(ID3D11Device* device, const char* fileName, bool triangulate = false, bool isRayCast = false, std::string* path = nullptr);

	// --- モデルの取得 ---
	MeshRenderer* GetModel(const char* fileName, std::string* path = nullptr, bool triangulate = false, bool isRayCast = false);

	// --- デバッグGuiの更新 ---
	void UpdateDebugGui();

private:
	Resource resources_;
};

