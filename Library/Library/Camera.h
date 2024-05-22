#pragma once

#include "../Math/Matrix.h"


class Camera {
private:
	Camera() {}
	~Camera() {}


public:
	// --- �C���X�^���X�̎擾 ---
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

	// --- �X�V���� ---
	void Update(float elapsedTime);

	// --- �ړ� ---
	void Move(float elapsedTime);

	// --- �Y�[�� ---
	void Zoom(float amount);

	// --- ��] ---
	void Rotate(float elapsedTime);

	// --- �f�o�b�O�pGUI�`�� ---
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