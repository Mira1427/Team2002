#pragma once

#include "../Math/Matrix.h"


class MeshRenderer;


struct HitResult
{
	Vector3 position_;
	Vector3 normal_;
	float distance_ = 0.0f;
	int materialIndex_ = 0;
};


// ===== �����蔻��N���X ===================================================================================================================
class Collision
{
public:
	// --- ��`���m�̌������� ---
	static bool IntersectRectangles(
		const Vector2& centerA, const Vector2& sizeA,
		const Vector2& centerB, const Vector2& sizeB
	);

	// --- �~���m�̌������� ---
	static bool IntersectCircles(
		const Vector2& centerA, float radiusA,
		const Vector2& centerB, float radiusB
	);

	// --- �����̓��m�̌������� ---
	static bool IntersectBoxes(
		const Vector3& positionA,
		const Vector3& sizeA,
		const Vector3& positionB,
		const Vector3& sizeB
	);

	// --- �����m�̌������� ---
	static bool IntersectSpheres(
		const Vector3& positionA,
		float radiusA,
		const Vector3& positionB,
		float radiusB
	);


	// --- �~�����m�̌������� ---
	static bool IntersectCapsules(
		const Vector3& positionA, float radiusA, float heightA,
		const Vector3& positionB, float radiusB, float heightB
	);


	// --- ���Ɨ����̂̏Փ˔��� ---
	static bool IntersectSphereBox(
		const Vector3& positionA, const float radius,
		const Vector3& positionB, const Vector3& sizeB, const Vector3& rotation
	);


	// --- ���C�ƃ��f���̏Փ˔��� ---
	static bool IntersectRayModel(
		const Vector3& start,
		const Vector3& end,
		MeshRenderer* model,
		HitResult& result
	);
};

