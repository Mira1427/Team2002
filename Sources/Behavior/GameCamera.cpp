#include "GameCamera.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/Library/Library.h"

#include "../../Sources/EventManager.h"


void GameCameraBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:
		obj->state_++;

	case 1:

	{
		CameraComponent* camera = obj->GetComponent<CameraComponent>();

		const float height = 175.0f;
		const float range = 100.0f; 
		const float rotateSpeed = DirectX::XMConvertToRadians(45.0f) * elapsedTime;

		switch (InputManager::Instance().state(0) & (Input::LEFT | Input::RIGHT))
		{
		case Input::LEFT:
			obj->transform_->rotation_.y -= rotateSpeed;
			break;
		case Input::RIGHT:
			obj->transform_->rotation_.y += rotateSpeed;
			break;
		}

		float s = sinf(obj->transform_->rotation_.y);
		float c = cosf(obj->transform_->rotation_.y);

		obj->transform_->position_.x = c * range;
		obj->transform_->position_.y = height;
		obj->transform_->position_.z = s * range;


		// --- �r���[�s��쐬 ---
		camera->view_.MakeLookAt(obj->transform_->position_, camera->target_);
		camera->invView_ = Matrix::Inverse(camera->view_);

		// --- �v���W�F�N�V�����s��쐬 ---
		float aspect = RootsLib::Window::GetWidth() / RootsLib::Window::GetHeight();
		camera->projection_.MakePerspective(camera->fov_, aspect, camera->nearZ_, camera->farZ_);

		// --- �r���[�v���W�F�N�V�����s��쐬 ---
		camera->viewProjection_ = camera->view_ * camera->projection_;
		camera->invViewProjection_ = Matrix::Inverse(camera->viewProjection_);
		camera->frontVec_ = Vector3::Normalize(camera->invView_.v_[2].xyz());
		camera->upVec_ = Vector3::Normalize(camera->invView_.v_[1].xyz());
		camera->rightVec_ = Vector3::Normalize(camera->invView_.v_[0].xyz());
	}

		break;
	}
}