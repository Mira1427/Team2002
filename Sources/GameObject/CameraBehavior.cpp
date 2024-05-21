#include "CameraBehavior.h"

#include "../Input/InputManager.h"

#include "../Library/Library.h"


// --- カメラ処理の基底クラス ---
void BaseCameraBehavior::Execute(GameObject* obj, float elapsedTime)
{
	auto* camera = obj->GetComponent<CameraComponent>();

	// --- ビュー行列作成 ---
	camera->view_.MakeLookAt(obj->transform_->position_, camera->target_);
	camera->invView_ = Matrix::Inverse(camera->view_);

	// --- プロジェクション行列作成 ---
	float aspect = RootsLib::Window::GetWidth() / RootsLib::Window::GetHeight();
	camera->projection_.MakePerspective(camera->fov_, aspect, camera->nearZ_, camera->farZ_);

	// --- ビュープロジェクション行列作成 ---
	camera->viewProjection_ = camera->view_ * camera->projection_;
	camera->invViewProjection_ = Matrix::Inverse(camera->viewProjection_);
	camera->frontVec_ = Vector3::Normalize(camera->invView_.v_[2].xyz());
	camera->upVec_ = Vector3::Normalize(camera->invView_.v_[1].xyz());
	camera->rightVec_ = Vector3::Normalize(camera->invView_.v_[0].xyz());
}


// --- デバッグのカメラ処理 ---
void DebugCameraBehavior::Execute(GameObject* obj, float elapsedTime)
{
	BaseCameraBehavior::Execute(obj, elapsedTime);

	ApplyControl(obj, elapsedTime);

}


void DebugCameraBehavior::ApplyControl(GameObject* obj, float elapsedTime)
{
	CameraComponent* camera = obj->GetComponent<CameraComponent>();
	InputManager& input = InputManager::instance();

	{
		obj->transform_->position_ += camera->frontVec_ * input.m_.wheel_;
		camera->target_ += camera->frontVec_ * input.m_.wheel_;
	}

	if (input.state(0) & input::RMB)
	{
		const float moveSpeed = 10.0f * elapsedTime;
		const float moveX = static_cast<float>(input.getCursorDeltaX() * moveSpeed);
		const float moveY = -static_cast<float>(input.getCursorDeltaY() * moveSpeed);

		obj->transform_->position_ -=  camera->rightVec_ * moveX;
		obj->transform_->position_ += -camera->upVec_	 * moveY;
		camera->target_ -=  camera->rightVec_ * moveX;
		camera->target_ += -camera->upVec_	  * moveY;
	}

	if (GetAsyncKeyState(VK_MENU) && input.state(0) & input::LMB)
	{
		const float rotateSpeed = 20.0f * elapsedTime;
		const float rotateX = -input.getCursorDeltaY() * rotateSpeed;
		const float rotateY = -input.getCursorDeltaX() * rotateSpeed;

		obj->transform_->rotation_.x -= rotateX;
		obj->transform_->rotation_.y += rotateY;
	}

	Vector3 rotation = obj->transform_->rotation_.ToRadian();

	Matrix T;
	T.MakeRotation(rotation.x, rotation.y, 0.0f);

	Vector3 front = { T._31, T._32, T._33 };
	front.Normalize();

	obj->transform_->position_.x = camera->target_.x + front.x  * camera->range_;
	obj->transform_->position_.y = camera->target_.y + front.y  * camera->range_;
	obj->transform_->position_.z = camera->target_.z + -front.z * camera->range_;
}