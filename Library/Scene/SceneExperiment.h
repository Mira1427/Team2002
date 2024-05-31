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


	GameObject* chara1_;
	GameObject* chara2_;
};

