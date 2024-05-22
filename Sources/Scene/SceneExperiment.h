#pragma once

#include "SceneManager.h"

#include "../Graphics/Primitive.h"
#include "../Graphics/MeshRenderer.h"
#include "../Graphics/SkyMap.h"
#include "../Graphics/PrimitiveBatch.h"
#include "../Graphics/GeometricPrimitiveBatch.h"
#include "../Graphics/InstancedMesh.h"
#include "../Graphics/Particle.h"

#include "../Algorithm/PerlinNoise.h"


class GameObject;

// ===== 実験用シーン ======================================================================================================================================================
class SceneExperiment final : public Scene
{
public:
	// --- 初期化 ---
	void Initialize() override;

	// --- 終了処理 ---
	void Finalize() override;

	// --- 更新処理 ---
	void Update(float elapsedTime) override;

	// --- 描画処理 ---
	void Render(ID3D11DeviceContext* dc) override;



	Vector2 position_ = { 640.0f, 360.0f };
	Vector2 size_ = { 100.0f, 100.0f };
	Vector2 center_ = Vector2::Zero_;
	float angle_ = 0.0f;
	Vector4 color_ = Vector4::White_;


	struct SpriteData
	{
		Vector3 position_;
		Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
		Vector2 texPos_;
		Vector2 texSize_ = { 400.0f, 400.0f };
		Vector2 center_;
		Vector3 rotation_;
		Vector4 color_ = Vector4::White_;
		bool inWorld_ = false;
		bool useBillboard_ = false;
	}data_;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
	D3D11_TEXTURE2D_DESC tex2dDesc_;

	PerlinNoise noise_;


	std::unique_ptr<MeshRenderer> skinnedMesh_;

	struct ModelData
	{
		Vector3 translation_;
		Vector3 scaling_ = { 1.0f, 1.0f, 1.0f };
		Vector3 rotation_;
		Vector4 color_ = Vector4::White_;
		int coordinateSystem_ = 0;
	}modelData_;

	ModelData modelData2_;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skinSrv_;
	D3D11_TEXTURE2D_DESC skinTex2dDesc_;


	std::unique_ptr<SkyMap> skyMap_;


	std::unique_ptr<PrimitiveBatch> primitiveBatch_;


	std::unique_ptr<GeometricPrimitiveBatch> geometricPrimitive_;


	int rasterState_;

	float noiseHeight_ = 18.0f;
	bool isDrawLandScape_ = false;

	std::shared_ptr<MeshRenderer> model_;
	std::shared_ptr<MeshRenderer> model2_;

	GameObject* obj_;
	GameObject* obj2_;
	GameObject* playerObj_;
	GameObject* enemyObj_;
	GameObject* pointLight_;

	std::unique_ptr<InstancedMesh> instancedMesh_;

	std::unique_ptr<Particle> particle_;
};

