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

// ===== �����p�V�[�� ======================================================================================================================================================
class SceneExperiment final : public Scene
{
public:
	// --- ������ ---
	void Initialize() override;

	// --- �I������ ---
	void Finalize() override;

	// --- �X�V���� ---
	void Update(float elapsedTime) override;

	// --- �`�揈�� ---
	void Render(ID3D11DeviceContext* dc) override;


	GameObject* chara1_;
	GameObject* chara2_;
};

