#pragma once

#include <list>

#include "../Math/Vector.h"


class LightingManager
{
public:
	LightingManager();
	~LightingManager() {};

	void Update();
	void UpdateDebugGui();


	// --- �A���r�G���g���C�g ---
	struct AmbientLight
	{
		Vector3 color_;	// �F
		float	pad_;
	};


	// --- �f�B���N�V�������C�g ---
	struct DirectionLight
	{
		Vector3 direction_;	// ����
		float	intensity_;	// ���x
		Vector3 color_;		// �F
		float	pad_;
		Vector3 viewFocus_;
		float   viewDistance_;
		float   viewSize_;
		float   viewNearZ_;
		float   viewFarZ_;
	};


	// --- �|�C���g���C�g ---
	struct PointLight
	{
		PointLight() {}
		PointLight(const Vector3& position, float intensity, const Vector3& color, float range) 
			: position_(position), intensity_(intensity), color_(color), range_(range) {}

		Vector3 position_;	// �ʒu
		float	intensity_;	// ���x
		Vector3 color_;		// �F
		float	range_;		// �͈�
	};


	AmbientLight ambientLight_;			// �A���r�G���g���C�g
	DirectionLight directionLight_;		// �f�B���N�V�������C�g
	std::list<PointLight> pointLights_;	// �|�C���g���C�g
};

