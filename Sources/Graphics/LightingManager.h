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


	// --- アンビエントライト ---
	struct AmbientLight
	{
		Vector3 color_;	// 色
		float	pad_;
	};


	// --- ディレクションライト ---
	struct DirectionLight
	{
		Vector3 direction_;	// 方向
		float	intensity_;	// 強度
		Vector3 color_;		// 色
		float	pad_;
		Vector3 viewFocus_;
		float   viewDistance_;
		float   viewSize_;
		float   viewNearZ_;
		float   viewFarZ_;
	};


	// --- ポイントライト ---
	struct PointLight
	{
		PointLight() {}
		PointLight(const Vector3& position, float intensity, const Vector3& color, float range) 
			: position_(position), intensity_(intensity), color_(color), range_(range) {}

		Vector3 position_;	// 位置
		float	intensity_;	// 強度
		Vector3 color_;		// 色
		float	range_;		// 範囲
	};


	AmbientLight ambientLight_;			// アンビエントライト
	DirectionLight directionLight_;		// ディレクションライト
	std::list<PointLight> pointLights_;	// ポイントライト
};

