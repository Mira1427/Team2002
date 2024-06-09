#include "GameCamera.h"

#include "../../External/ImGui/imgui.h"

#include "../../Library/Input/InputManager.h"

#include "../../Library/Library/Library.h"

#include "../../Sources/EventManager.h"
#include "../../Sources/Component/Component.h"


void GameCameraBehavior::Execute(GameObject* obj, float elapsedTime)
{
	if (EventManager::Instance().paused_)
		return;

	switch (obj->state_)
	{
	case 0:

	{
		CameraShakeComponent* shake = obj->GetComponent<CameraShakeComponent>();
		shake->shakeIntensity_ = { 5.0f, 5.0f, 3.0f };
	}

		obj->state_++;

	case 1:
	{

		obj->transform_->position_.y = 1.0f;
		CameraComponent* camera = obj->GetComponent<CameraComponent>();


#ifdef USE_IMGUI
		static float dist = 26.0f;
		static float offset = 11.0f;
		static float height = 3.0f;
		static float horizontalOffset = 8.0f;
		ImGui::DragFloat(u8"距離", &dist);
		ImGui::DragFloat(u8"オフセット", &offset);
		ImGui::DragFloat(u8"高さ", &height);
		ImGui::DragFloat(u8"横", &horizontalOffset);
#else
		const float dist = 26.0f;
		const float offset = 11.0f;
		const float height = 3.0f;
		const float horizontalOffset = 8.0f;
#endif

		const float rotateSpeed = DirectX::XMConvertToRadians(22.5f) * elapsedTime;
		obj->transform_->rotation_.y -= rotateSpeed;


		// --- カメラの位置を自機の斜め手前に固定 ---
		auto* player = EventManager::Instance().controller_->child_[0];

		Matrix R;
		R.MakeRotation(player->transform_->rotation_.ToRadian());
		Vector3 offsetVec = Vector3::Normalize(-R.v_[2].xyz() + -R.v_[0].xyz()) * offset + Vector3::Up_ * height;

		obj->transform_->position_ = Vector3::Normalize(-R.v_[2].xyz() + R.v_[0].xyz()) * dist + player->transform_->position_ + offsetVec + R.v_[0].xyz() * horizontalOffset;
		camera->target_ = player->transform_->position_ + offsetVec;	// 自機の方を向く


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

		break;

	case 2:
	{
		CameraComponent* camera = obj->GetComponent<CameraComponent>();

		static float height = 190.0f;
		static float range = 50.0f; 
		const float rotateSpeed = DirectX::XMConvertToRadians(45.0f) * elapsedTime;

#ifdef USE_IMGUI
		//ImGui::DragFloat(u8"高さ", &height);
		//ImGui::DragFloat(u8"距離", &range);
#endif

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


		CameraShakeComponent* shake = obj->GetComponent<CameraShakeComponent>();
		ShakeCamera(obj, camera, shake, elapsedTime);


		// --- ビュー行列作成 ---
		camera->view_.MakeLookAt(obj->transform_->position_ + shake->offset_, camera->target_);
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

		break;
	}
}


void GameCameraBehavior::ShakeCamera(GameObject* obj, CameraComponent* camera, CameraShakeComponent* shake, float elapsedTime)
{
	obj->timer_ -= elapsedTime;
	if (obj->timer_ > 0.0f)
	{
		shake->offset_ = camera->rightVec_ * (((rand() % 100) / 100.0f) - 0.5f) * shake->shakeIntensity_.x;
		shake->offset_ += camera->upVec_ * (((rand() % 100) / 100.0f) - 0.5f) * shake->shakeIntensity_.y;
		shake->offset_ += camera->frontVec_ * (((rand() % 100) / 100.0f) - 0.5f) * shake->shakeIntensity_.z;
	}

	else
	{
		obj->timer_ = 0.0f;
		shake->offset_ = Vector3::Zero_;
	}

}
