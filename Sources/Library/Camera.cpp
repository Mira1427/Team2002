#include "Camera.h"

#include "../../External/ImGui/imgui.h"

#include "../Input/InputManager.h"

#include "Library.h"

// ===== カメラクラス ========================================================================================================================

// --- 更新処理 ---
void Camera::Update(float elapsedTime)
{
	// --- ビュー行列作成 ---
	view_.MakeLookAt(position_, target_);
	invView_ = Matrix::Inverse(view_);

	// --- プロジェクション行列作成 ---
	float aspect = RootsLib::Window::GetWidth() / RootsLib::Window::GetHeight();
	projection_.MakePerspective(fov_, aspect, nearZ_, farZ_);

	// --- ビュープロジェクション行列作成 ---
	viewProjection_ = view_ * projection_;


	// --- 逆行列からカメラの正面、上、右方向のベクトルを取得 ---
	Matrix world = Matrix::Inverse(view_);
	frontVec_ = Vector3::Normalize({ world._31, world._32, world._33 });
	upVec_ = Vector3::Normalize({ world._21, world._22, world._23 });
	rightVec_ = Vector3::Normalize({ world._11, world._12, world._13 });


	// --- 移動 ---
	Move(elapsedTime);

	// --- 回転 ---
	Rotate(elapsedTime);
}


// --- 移動 ---
void Camera::Move(float elapsedTime)
{
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		auto& input = InputManager::instance();
		
		const float moveSpeed = 10.0f * elapsedTime;
		position_	+= rightVec_ * static_cast<float>(input.getCursorDeltaX()) * moveSpeed;
		target_		+= rightVec_ * static_cast<float>(input.getCursorDeltaX()) * moveSpeed;
		position_	+= -upVec_	 * static_cast<float>(input.getCursorDeltaY()) * moveSpeed;
		target_		+= -upVec_	 * static_cast<float>(input.getCursorDeltaY()) * moveSpeed;
	}
}


// --- ズーム ---
void Camera::Zoom(float amount)
{
	position_ += frontVec_ * amount;
	target_ += frontVec_ * amount;
}


// --- 回転 ---
void Camera::Rotate(float elapsedTime)
{
	if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_LBUTTON))
	{
		auto& input = InputManager::instance();

		static Vector3 angle{ 150, 0.0f, 0.0f };

		const float rotateSpeed = 5.0f * elapsedTime;

		angle.x += -input.getCursorDeltaY() * rotateSpeed;
		angle.y += -input.getCursorDeltaX() * rotateSpeed;
		angle.ToRadian();

		Matrix T;
		T.MakeRotation(angle.x, angle.y, 0.0f);

		Vector3 front = { T._31, T._32, T._33 };
		front.Normalize();

		position_.x = target_.x + front.x * 15;
		position_.y = target_.y + front.y * 15;
		position_.z = target_.z + -front.z * 15;
	}
}


// --- デバッグ用GUI描画 ---
void Camera::UpdateDebugGui()
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(u8"ビュー行列"))
	{
		ImGui::DragFloat3(u8"位置", &position_.x, 0.1f, -10000, 10000);
		ImGui::DragFloat3(u8"注視点", &target_.x, 0.1f, -10000, 10000);
		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(u8"プロジェクション行列"))
	{
		ImGui::DragFloat(u8"視野角", &fov_, 1.0f, 60.0f, 120.0f);
		ImGui::DragFloat(u8"最近距離", &nearZ_, 0.1f, 0.0f, 1000.0f);
		ImGui::DragFloat(u8"最遠距離", &farZ_, 0.1f, 0.0f, 1000.0f);
		ImGui::TreePop();
	}
}
