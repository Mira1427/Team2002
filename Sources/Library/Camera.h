#pragma once

#include "../Math/Matrix.h"


class Camera {
private:
	Camera() {}
	~Camera() {}


public:
	// --- インスタンスの取得 ---
	static Camera& Instance() {
		static Camera camera;
		return camera;
	}

	const Matrix&	GetViewProjection() const	{ return viewProjection_; }
	const Matrix&	GetView() const				{ return view_; }
	const Matrix	GetProjection() const		{ return projection_; }
	const Matrix&	GetInvView() const			{ return invView_; }
	const Vector3&	GetPosition() const			{ return position_; }
	const Vector3&	GetTarget() const			{ return target_; }

	// --- 更新処理 ---
	void Update(float elapsedTime);

	// --- 移動 ---
	void Move(float elapsedTime);

	// --- ズーム ---
	void Zoom(float amount);

	// --- 回転 ---
	void Rotate(float elapsedTime);

	// --- デバッグ用GUI描画 ---
	void UpdateDebugGui();

private:
	Matrix viewProjection_;
	Matrix view_;
	Matrix invView_;
	Matrix projection_;

	Vector3 position_{ 0.0f, 10.0f, -10.0f };
	Vector3 target_;

	float fov_ = 60.0f;
	float nearZ_ = 0.1f;
	float farZ_ = 1000.0f;

	Vector3 frontVec_;
	Vector3 upVec_;
	Vector3 rightVec_;
};