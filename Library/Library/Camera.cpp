#include "Camera.h"

#include "../../External/ImGui/imgui.h"

#include "../Input/InputManager.h"

#include "Library.h"

// ===== �J�����N���X ========================================================================================================================

// --- �X�V���� ---
void Camera::Update(float elapsedTime)
{
	// --- �r���[�s��쐬 ---
	view_.MakeLookAt(position_, target_);
	invView_ = Matrix::Inverse(view_);

	// --- �v���W�F�N�V�����s��쐬 ---
	float aspect = RootsLib::Window::GetWidth() / RootsLib::Window::GetHeight();
	projection_.MakePerspective(fov_, aspect, nearZ_, farZ_);

	// --- �r���[�v���W�F�N�V�����s��쐬 ---
	viewProjection_ = view_ * projection_;


	// --- �t�s�񂩂�J�����̐��ʁA��A�E�����̃x�N�g�����擾 ---
	Matrix world = Matrix::Inverse(view_);
	frontVec_ = Vector3::Normalize({ world._31, world._32, world._33 });
	upVec_ = Vector3::Normalize({ world._21, world._22, world._23 });
	rightVec_ = Vector3::Normalize({ world._11, world._12, world._13 });


	// --- �ړ� ---
	Move(elapsedTime);

	// --- ��] ---
	Rotate(elapsedTime);
}


// --- �ړ� ---
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


// --- �Y�[�� ---
void Camera::Zoom(float amount)
{
	position_ += frontVec_ * amount;
	target_ += frontVec_ * amount;
}


// --- ��] ---
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


// --- �f�o�b�O�pGUI�`�� ---
void Camera::UpdateDebugGui()
{
	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(u8"�r���[�s��"))
	{
		ImGui::DragFloat3(u8"�ʒu", &position_.x, 0.1f, -10000, 10000);
		ImGui::DragFloat3(u8"�����_", &target_.x, 0.1f, -10000, 10000);
		ImGui::TreePop();
	}

	ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
	if (ImGui::TreeNode(u8"�v���W�F�N�V�����s��"))
	{
		ImGui::DragFloat(u8"����p", &fov_, 1.0f, 60.0f, 120.0f);
		ImGui::DragFloat(u8"�ŋߋ���", &nearZ_, 0.1f, 0.0f, 1000.0f);
		ImGui::DragFloat(u8"�ŉ�����", &farZ_, 0.1f, 0.0f, 1000.0f);
		ImGui::TreePop();
	}
}
