#include "GameObject.h"

#include "../../External/ImGui/imgui.h"

#include "../Graphics/LightingManager.h"
#include "../Graphics/ModelManager.h"

#include "../Library/library.h"


// ===== �R���|�[�l���g =======================================================================================================================================

// --- ���W�n�̕ϊ��p�̍s�� ---
const Matrix Transform::CoordinateSystems_[4] =
{
	// --- �E��n�@Y���A�b�v ---
	{
		-1, 0, 0, 0,
		 0, 1, 0, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 1
	},

	// --- ����n�@Y���A�b�v ---
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	},

	// --- �E��n�@Z���A�b�v ---
	{
		-1, 0,  0, 0,
		 0, 0, -1, 0,
		 0, 1,  0, 0,
		 0, 0,  0, 1
	},

	// --- ����n�@Z���A�b�v ---
	{
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	}
};


// --- ���[���h���W�ւ̕ϊ� ---
void Transform::MakeTransform()
{
	Matrix translation;
	translation.MakeTranslation(position_);

	Matrix scaling;
	scaling.MakeScaling(scaling_);

	Matrix rotation;
	Quaternion orientation;
	orientation.SetRotationDegFromVector(rotation_);
	rotation.MakeRotationFromQuaternion(orientation);

	world_ = CoordinateSystems_[coordinateSystem_] * scaling * rotation * translation;
}


// --- �f�o�b�OGui�X�V ---
void Transform::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Transform")) {
		ImGui::Spacing();

		const char* coordinateNames[4] =
		{ u8"�E��n�@Y���A�b�v", u8"����n�@Y���A�b�v", u8"�E��n�@Z���A�b�v", u8"����n�@Z���A�b�v" };

		ImGui::Combo(u8"���W�n", &coordinateSystem_, coordinateNames, 4);


		ImGui::DragFloat4(u8"�ʒu", &position_.x, moveSpeed_);

		static bool isUniform;
		if (ImGui::DragFloat4(u8"�X�P�[��", &scaling_.x, scaleSpeed_))
		{
			if (isUniform)
				scaling_.z = scaling_.y = scaling_.x;
		}
		ImGui::SameLine();
		ImGui::Checkbox("##bool", &isUniform);

		ImGui::DragFloat4(u8"��]", &rotation_.x, DirectX::XMConvertToRadians(rotateSpeed_));

		ImGui::TreePop();
	}
}



// ===== 2D�摜�`�� ======================================================================================================================================================
void SpriteRendererComponent::Draw(ID3D11DeviceContext* dc)
{
	if (!texture_)
		return;

	if (!isVisible_)
		return;

	RootsLib::Blender::SetState(static_cast<BlendState>(blendState_));
	RootsLib::Raster::SetState(static_cast<RasterState>(rasterState_));
	RootsLib::Depth::SetState(static_cast<DepthState>(testDepth_), static_cast<DepthState>(writeDepth_));

	Graphics::Instance().GetSpriteRenderer()->Draw(
		dc,
		*texture_,
		object_->transform_->position_,
		object_->transform_->scaling_,
		texPos_,
		texSize_,
		center_,
		object_->transform_->rotation_,
		color_,
		inWorld_,
		useBillboard_
	);


	if (object_->isChoose_ && !useBillboard_ && !inWorld_)
	{
		Vector2 size = { texSize_.x * object_->transform_->scaling_.x, texSize_.y * object_->transform_->scaling_.y };

		Graphics::Instance().GetDebugRenderer()->DrawRectangle(
			object_->transform_->position_.xy(),
			size,
			center_,
			object_->transform_->rotation_.x,
			{ 1.0f, 1.0f, 0.0f, 1.0f }
		);
	}
}


// --- �f�o�b�OGui�̍X�V ---
void SpriteRendererComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("SpriteRenderer")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		// --- �e�N�X�`���̒��ւ� ---
		ImGui::InputText(u8"�e�N�X�`��", textureName_.data(), textureName_.size());
		ImGui::SameLine();
		if (ImGui::Button(u8". . .", ImVec2(30.0f, 20.0f)))
		{
			std::wstring str;
			texture_ = TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), RootsLib::File::GetFileName().c_str(), &str);
			texSize_ = { texture_->width_, texture_->height_ };
			textureName_ = RootsLib::String::ConvertWideChar(str.c_str());
		}


		// --- �u�����h�X�e�[�g ---
		static const char* blendStates[] = { u8"�Ȃ�", u8"����", u8"���Z", u8"���Z" };
		ImGui::Combo(u8"�u�����h", &blendState_, blendStates, ARRAYSIZE(blendStates));

		// --- ���X�^�[�X�e�[�g ---
		static const char* rasterStates[] = { u8"�J�����O�Ȃ�", u8"�w�ʃJ�����O", u8"�O�ʃJ�����O", u8"���C���[�t���[��", u8"���C���[�t���[�� �J�����O�Ȃ�" };
		ImGui::Combo("���X�^�[", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

		// --- �[�x�X�e�[�g ---
		ImGui::Checkbox(u8"�[�x�e�X�g", &testDepth_);
		ImGui::Checkbox(u8"�[�x��������", &writeDepth_);

		ImGui::DragFloat2(u8"�؂蔲���ʒu", &texPos_.x);
		ImGui::DragFloat2(u8"�؂蔲���T�C�Y", &texSize_.x);
		ImGui::DragFloat2(u8"���S", &center_.x);
		ImGui::ColorEdit4(u8"�F", &color_.x);
		ImGui::Checkbox(u8"���[���h���", &inWorld_);
		ImGui::Checkbox(u8"�r���{�[�h", &useBillboard_);

		ImGui::TreePop();
	}
}


// --- 2D�摜�̃A�j���[�V�����X�V ---
void AnimationComponent::Update(float elapsedTime)
{
	SpriteRendererComponent* spriteRenderer = object_->GetComponent<SpriteRendererComponent>();

	if (!spriteRenderer)
		return;

	if (!animationClips_)
		return;

	if (animationClips_->size() == 0)
		return;

	AnimationData& animeData = animationClips_->at(static_cast<size_t>(animationIndex));

	spriteRenderer->texture_ = animeData.texture_;

	if (!spriteRenderer->texture_)
		return;

	endFrame_ = static_cast<int>(animeData.frameData_->size() - 1);


	// --- �Đ��� ---
	if (isPlay_)
	{
		frame_ += timeScale_ * elapsedTime;
	}

	// --- �I���t���[���ɓ��B������ ---
	if (frame_ > animeData.frameData_->at(static_cast<size_t>(frameIndex)).frame_)
	{
		frameIndex++;
		frame_ = 0.0f;

		if (frameIndex > endFrame_)
		{
			if (isLoop_)
			{
				frameIndex = startFrame_;
			}

			else
			{
				isPlay_ = false;
				frameIndex = endFrame_;
			}
		}
	}

	spriteRenderer->texPos_ = animeData.frameData_->at(static_cast<size_t>(frameIndex)).texPos_;
	spriteRenderer->texSize_ = animeData.frameData_->at(static_cast<size_t>(frameIndex)).texSize_;
}

void AnimationComponent::UpdateDebugGui(float elapsedTime)
{
	SpriteRendererComponent* spriteRenderer = object_->GetComponent<SpriteRendererComponent>();

	if (!spriteRenderer)
	{
		ImGui::Text(u8"�R���|�[�l���g������܂���");
		return;
	}

	if (!spriteRenderer->texture_)
	{
		ImGui::Text(u8"�e�N�X�`��������܂���");
		return;
	}

	if (!animationClips_)
	{
		ImGui::Text(u8"�A�j���[�V����������܂���");
		return;
	}

	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Animation")) {
		ImGui::Spacing();

		std::vector<const char*> animeNames;

		for (auto& animation : *animationClips_)
			animeNames.emplace_back(animation.name_.c_str());


		if (ImGui::Combo(u8"�A�j���[�V����", &animationIndex, animeNames.data(), static_cast<int>(animeNames.size())))
		{
			frameIndex = 0;
			frame_ = 0.0f;
		}

		if (ImGui::Button(u8"�Đ�", { 35, 20 }))
		{
			isPlay_ = true;
			frame_ = static_cast<float>(startFrame_);
			frameIndex = startFrame_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"�ꎞ��~", { 60, 20 }))
		{
			isPlay_ = !isPlay_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"��~", { 35, 20 }))
		{
			isPlay_ = false;
		}
		ImGui::SameLine();
		ImGui::Checkbox(u8"���[�v", &isLoop_);


		ImGui::DragFloat(u8"�^�C���X�P�[��", &timeScale_, 0.1f);
		ImGui::InputInt(u8"�t���[��", &frameIndex);
		ImGui::InputInt(u8"�J�n�t���[��", &startFrame_);
		ImGui::InputInt(u8"�I���t���[��", &endFrame_);

		ImGui::TreePop();
	}

}


// --- ���f���̕`�� ---
void MeshRendererComponent::Draw(ID3D11DeviceContext* dc)
{
	if (!model_)
		return;

	if (!isVisible_)
		return;

	AnimatorComponent* animator = object_->GetComponent<AnimatorComponent>();

	RootsLib::Blender::SetState(static_cast<BlendState>(blendState_));
	RootsLib::Raster::SetState(static_cast<RasterState>(rasterState_));
	RootsLib::Depth::SetState(static_cast<DepthState>(testDepth_), static_cast<DepthState>(writeDepth_));

	if (animator)
	{
		ID3D11PixelShader* nullPS = NULL;

		model_->render(
			dc,
			object_->transform_->world_.mat_,
			color_.vec_,
			((animator->keyFrame_) ? animator->keyFrame_ : nullptr),
			(GameObjectManager::Instance().castShadow_) ? &nullPS : nullptr
		);
	}

	else
	{
		ID3D11PixelShader* nullPS = NULL;

		model_->render(
			dc,
			object_->transform_->world_.mat_,
			color_.vec_,
			nullptr,
			(GameObjectManager::Instance().castShadow_) ? &nullPS : nullptr
		);
	}
}

void MeshRendererComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("MeshRenderer")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::InputText(u8"�ǂݍ���", modelName_.data(), 1024);
		ImGui::SameLine();
		if (ImGui::Button(". . .", ImVec2(30.0f, 20.0f)))
		{
			model_ = ModelManager::Instance().GetModel(modelName_.c_str());
		}


		if (model_)
		{

			std::vector<const char*> boneNames;
			std::vector<const char*> meshNames;

			static const char* blendStates[] = { u8"�Ȃ�", u8"����", u8"���Z", u8"���Z" };
			ImGui::Combo(u8"�u�����h", &blendState_, blendStates, ARRAYSIZE(blendStates));

			static const char* rasterStates[] = { u8"�J�����O�Ȃ�", u8"�w�ʃJ�����O", u8"�O�ʃJ�����O", u8"���C���[�t���[��", u8"���C���[�t���[�� �J�����O�Ȃ�" };
			ImGui::Combo(u8"���X�^�[", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

			ImGui::Checkbox(u8"�[�x�e�X�g", &testDepth_);
			ImGui::Checkbox(u8"�[�x��������", &writeDepth_);


			ImGui::ColorEdit4(u8"�F", &color_.r);


			for (auto& mesh : model_->meshes)
				meshNames.emplace_back(mesh.name.c_str());

			if (ImGui::Combo(u8"���b�V��", &meshIndex, meshNames.data(), static_cast<int>(meshNames.size())))
				boneIndex = 0;

			for (auto& bone : model_->meshes.at(meshIndex).bindPose.bones)
				boneNames.emplace_back(bone.name.c_str());

			ImGui::Combo(u8"�{�[��", &boneIndex, boneNames.data(), static_cast<int>(boneNames.size()));

			if (ImGui::TreeNode(u8"�}�e���A��"))
			{
				ImGui::Text(u8"�}�e���A���̐� : %d", static_cast<int>(model_->materials.size()));

				static const char* materialTypes[static_cast<size_t>(MaterialLabel::MAX)] =
				{
					u8"�J���[", u8"�@��", u8"�G�~�b�V�u", u8"���t�l�X", u8"���^���b�N", u8"�A���r�G���g�I�N���[�W����"
				};

				// --- �}�e���A���̕\�� ---
				for (auto& material : model_->materials)
				{
					for (size_t i = 0; i < static_cast<size_t>(MaterialLabel::MAX); i++)
					{
						ImGui::Separator();
						ImGui::BulletText(materialTypes[i]);
						ImGui::SameLine();

						// --- �}�e���A���̕ύX ---
						std::string str = u8". . .##" + std::to_string(material.second.uniqueID) + materialTypes[i];
						if (ImGui::Button(str.c_str(), ImVec2(30.0f, 20.0f)))
						{
							std::wstring wstr;
							Texture* texture = TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), RootsLib::File::GetFileName().c_str(), &wstr);
							material.second.shaderResourceViews[i] = texture->srv_;
							material.second.textureFileNames[i] = RootsLib::String::ConvertWideChar(wstr.c_str()).c_str();
						}
						ImGui::Text(material.second.textureFileNames[i].c_str());

						// --- �}�e���A���̕\�� ---
						ImGui::Image(material.second.shaderResourceViews[i].Get(), ImVec2(250.0f, 250.0f));
					}
				}


				ImGui::TreePop();
			}
		}

		else
		{
			ImGui::Text(u8"���f�����ǂݍ��܂�Ă��܂���");
		}

		ImGui::TreePop();
	}
}


// --- ���f���̃A�j���[�V�����̍X�V ---
void AnimatorComponent::Update(float elapsedTime)
{
	MeshRendererComponent* meshRenderer = object_->GetComponent<MeshRendererComponent>();

	// --- �R���|�[�l���g�����邩�ǂ��� ---
	if (!meshRenderer)
		return;

	// --- ���f�����ǂݍ��܂�Ă��邩�ǂ��� ---
	if (!meshRenderer->model_)
		return;

	// --- �A�j���[�V���������邩 ---
	if (meshRenderer->model_->animationClips.size() < 1)
		return;

	Animation& animation{ meshRenderer->model_->animationClips.at(static_cast<size_t>(animationIndex)) };
	endFrame_ = static_cast<int>(animation.sequence.size() - 1);

	// --- �Đ��� ---
	if (isPlay_)
	{
		frame_ += timeScale_ * elapsedTime;
		keyFrameIndex = static_cast<int>(frame_);
	}

	// --- �I���t���[���ɓ��B������ ---
	if (keyFrameIndex > endFrame_)
	{
		// --- ���[�v�� ---
		if (isLoop_)
		{
			// --- �J�n�t���[���ɖ߂� ---
			frame_ = static_cast<float>(startFrame_);
			keyFrameIndex = startFrame_;
		}

		// --- ����ȊO ---
		else
		{
			// --- �Đ����~���āA�I���t���[���Ŏ~�߂� ---
			isPlay_ = false;
			frame_ = static_cast<float>(endFrame_);
			keyFrameIndex = endFrame_;
		}
	}

	keyFrame_ = &animation.sequence.at(static_cast<size_t>(keyFrameIndex));
}


// --- Mesh�̃m�[�h�̍X�V ---
void AnimatorComponent::UpdateNodeTransform()
{
	MeshRendererComponent* meshRenderer = object_->GetComponent<MeshRendererComponent>();

	// --- �R���|�[�l���g�����邩�ǂ��� ---
	if (!meshRenderer)
		return;

	// --- ���f�����ǂݍ��܂�Ă��邩�ǂ��� ---
	if (!meshRenderer->model_)
		return;

	auto& node = keyFrame_->nodes.at(nodeIndex);
	node.translation = translation_.vec_;
	node.scaling = scaling_.vec_;
	Quaternion orientation;
	orientation.SetRotationDegFromVector(rotation_);
	node.rotation = orientation.vec_;


	meshRenderer->model_->updateAnimation(*keyFrame_);
}


// --- �A�j���[�^�[�̃f�o�b�OGui�̍X�V ---
void AnimatorComponent::UpdateDebugGui(float elapsedTime)
{
	MeshRendererComponent* meshRenderer = object_->GetComponent<MeshRendererComponent>();

	if (!meshRenderer)
	{
		ImGui::Text(u8"�R���|�[�l���g������܂���");
		return;
	}

	if (!meshRenderer->model_)
	{
		ImGui::Text(u8"���f�����ǂݍ��܂�Ă��܂���");
		return;
	}


	// --- �A�j���[�V���������邩 ---
	if (meshRenderer->model_->animationClips.size() < 1)
	{
		ImGui::Text(u8"�A�j���[�V����������܂���");
		return;
	}

	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode(u8"Animator")) {
		ImGui::Spacing();

		std::vector<const char*> animeNames;

		for (auto& animation : meshRenderer->model_->animationClips)
			animeNames.emplace_back(animation.name.c_str());


		if (ImGui::Combo(u8"�A�j���[�V����", &animationIndex, animeNames.data(), static_cast<int>(animeNames.size())))
		{
			keyFrameIndex = 0;
			frame_ = 0.0f;
		}

		if (ImGui::Button(u8"�Đ�", { 35, 20 }))
		{
			isPlay_ = true;
			frame_ = static_cast<float>(startFrame_);
			keyFrameIndex = startFrame_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"�ꎞ��~", { 60, 20 }))
		{
			isPlay_ = !isPlay_;
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"��~", { 35, 20 }))
		{
			isPlay_ = false;
		}
		ImGui::SameLine();
		ImGui::Checkbox(u8"���[�v", &isLoop_);


		ImGui::DragFloat(u8"�^�C���X�P�[��", &timeScale_, 0.1f);
		ImGui::InputInt(u8"�t���[��", &keyFrameIndex);
		ImGui::InputInt(u8"�J�n�t���[��", &startFrame_);
		ImGui::InputInt(u8"�I���t���[��", &endFrame_);


		// --- �m�[�h�̃f�o�b�O ---
		if (keyFrame_)
		{
			ImGui::Spacing();

			std::vector<const char*> nodeNames;

			for (auto& node : meshRenderer->model_->sceneView.nodes)
				nodeNames.emplace_back(node.name.c_str());

			if (ImGui::Combo(u8"�m�[�h", &nodeIndex, nodeNames.data(), static_cast<int>(nodeNames.size())))
			{
				auto& node = keyFrame_->nodes.at(nodeIndex);
				translation_.vec_ = node.translation;
				scaling_.vec_ = node.scaling;
				rotation_ = { node.rotation.x, node.rotation.y, node.rotation.z };
			}

			ImGui::DragFloat3(u8"�ʒu", &translation_.x);

			static bool isUniform;
			if (ImGui::DragFloat3(u8"�X�P�[��", &scaling_.x, 0.01f))
			{
				if (isUniform)
					scaling_.z = scaling_.y = scaling_.x;
			}
			ImGui::SameLine();
			ImGui::Checkbox("##bool", &isUniform);

			ImGui::DragFloat3(u8"��]", &rotation_.x, DirectX::XMConvertToRadians(5));

		}


		ImGui::TreePop();
	}
}



// ===== �C���X�^���X���b�V�� ======================================================================================================================================================
void InstancedMeshComponent::Draw(ID3D11DeviceContext* dc)
{
	if (!model_)
		return;

	if (!isVisible_)
		return;

	// --- �`�惊�X�g�ɒǉ� ---
	model_->AddDrawList(object_->transform_->world_, color_);
}

void InstancedMeshComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("InstancedMesh")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::InputText(u8"�ǂݍ���", modelName_.data(), 1024);
		ImGui::SameLine();
		if (ImGui::Button(". . .", ImVec2(30.0f, 20.0f)))
		{
			model_ = ModelManager::Instance().GetInstancedMesh(modelName_.c_str(), 100);
		}


		if (model_)
		{

			std::vector<const char*> boneNames;
			std::vector<const char*> meshNames;

			static const char* blendStates[] = { u8"�Ȃ�", u8"����", u8"���Z", u8"���Z" };
			ImGui::Combo(u8"�u�����h", &blendState_, blendStates, ARRAYSIZE(blendStates));

			static const char* rasterStates[] = { u8"�J�����O�Ȃ�", u8"�w�ʃJ�����O", u8"�O�ʃJ�����O", u8"���C���[�t���[��", u8"���C���[�t���[�� �J�����O�Ȃ�" };
			ImGui::Combo(u8"���X�^�[", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

			ImGui::Checkbox(u8"�[�x�e�X�g", &testDepth_);
			ImGui::Checkbox(u8"�[�x��������", &writeDepth_);

			ImGui::ColorEdit4(u8"�F", &color_.r);


			for (auto& mesh : model_->meshes_)
				meshNames.emplace_back(mesh.name_.c_str());

			if (ImGui::Combo(u8"���b�V��", &meshIndex, meshNames.data(), static_cast<int>(meshNames.size())))

				if (ImGui::TreeNode(u8"�}�e���A��"))
				{
					ImGui::Text(u8"�}�e���A���̐� : %d", static_cast<int>(model_->materials_.size()));

					static const char* materialTypes[static_cast<size_t>(MaterialLabel::MAX)] =
					{
						u8"�J���[", u8"�@��", u8"�G�~�b�V�u", u8"���t�l�X", u8"���^���b�N", u8"�A���r�G���g�I�N���[�W����"
					};

					// --- �}�e���A���̕\�� ---
					for (auto& material : model_->materials_)
					{
						for (size_t i = 0; i < static_cast<size_t>(MaterialLabel::MAX); i++)
						{
							ImGui::Separator();
							ImGui::BulletText(materialTypes[i]);
							ImGui::SameLine();

							// --- �}�e���A���̕ύX ---
							std::string str = u8". . .##" + std::to_string(material.second.uniqueID) + materialTypes[i];
							if (ImGui::Button(str.c_str(), ImVec2(30.0f, 20.0f)))
							{
								std::wstring wstr;
								Texture* texture = TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), RootsLib::File::GetFileName().c_str(), &wstr);
								material.second.shaderResourceViews[i] = texture->srv_;
								material.second.textureFileNames[i] = RootsLib::String::ConvertWideChar(wstr.c_str()).c_str();
							}
							ImGui::Text(material.second.textureFileNames[i].c_str());

							// --- �}�e���A���̕\�� ---
							ImGui::Image(material.second.shaderResourceViews[i].Get(), ImVec2(250.0f, 250.0f));
						}
					}


					ImGui::TreePop();
				}
		}

		else
		{
			ImGui::Text(u8"���f�����ǂݍ��܂�Ă��܂���");
		}

		ImGui::TreePop();
	}
}



// --- �v���~�e�B�u�̕`�揈�� ---
void PrimitiveRendererComponent::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	RootsLib::Blender::SetState(static_cast<BlendState>(blendState_));
	RootsLib::Raster::SetState(static_cast<RasterState>(rasterState_));
	RootsLib::Depth::SetState(static_cast<DepthState>(testDepth_), static_cast<DepthState>(writeDepth_));

	Vector2 scale = object_->transform_->scaling_.xy();

	Graphics::Instance().GetPrimitiveRenderer()->Draw(
		dc,
		object_->transform_->position_.xy(),
		{ size_.x * scale.x, size_.y * scale.y },
		center_,
		object_->transform_->rotation_.x,
		color_
	);
}

void PrimitiveRendererComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("PrimitiveRenderer")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		static const char* blendStates[] = { u8"�Ȃ�", u8"����", u8"���Z", u8"���Z" };
		ImGui::Combo(u8"�u�����h", &blendState_, blendStates, ARRAYSIZE(blendStates));

		static const char* rasterStates[] = { u8"�J�����O�Ȃ�", u8"�w�ʃJ�����O", u8"�O�ʃJ�����O", u8"���C���[�t���[��", u8"���C���[�t���[�� �J�����O�Ȃ�" };
		ImGui::Combo("���X�^�[", &rasterState_, rasterStates, ARRAYSIZE(rasterStates));

		ImGui::Checkbox(u8"�[�x�e�X�g", &testDepth_);
		ImGui::Checkbox(u8"�[�x��������", &writeDepth_);

		ImGui::DragFloat2(u8"�T�C�Y", &size_.x);
		ImGui::DragFloat2(u8"���S", &center_.x);
		ImGui::ColorEdit4(u8"�F", &color_.x);

		ImGui::TreePop();
	}

}


// --- 2D�̋�`�̔���̈�̕`�� ---
void BoxCollider2D::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Vector2 position = object_->transform_->position_.xy() + offset_;

	Graphics::Instance().GetDebugRenderer()->DrawRectangle(
		position,
		size_,
		{ 0.0f, 0.0f },
		0.0f,
		Vector4::White_
	);
}

void BoxCollider2D::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("BoxCollider2D")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::Checkbox(u8"���茋��", &isHit_);
		ImGui::DragFloat2(u8"�T�C�Y", &size_.x, 1.0f);
		ImGui::DragFloat2(u8"�I�t�Z�b�g", &offset_.x, 1.0f);

		ImGui::TreePop();
	}

}


// --- 2D�̉~�̔���̈�̕`�� ---
void CircleCollider::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Vector2 position = object_->transform_->position_.xy() + offset_;

	Graphics::Instance().GetDebugRenderer()->DrawCircle(
		position,
		radius_,
		Vector4::White_
	);
}

void CircleCollider::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("CircleCollider")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::Checkbox(u8"���茋��", &isHit_);
		ImGui::DragFloat(u8"���a", &radius_, 1.0f);
		ImGui::DragFloat2(u8"�I�t�Z�b�g", &offset_.x, 1.0f);

		ImGui::TreePop();
	}

}


// --- �����̂̔���̈�̕`�� ---
void BoxCollider::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::Instance().GetDebugRenderer()->DrawCube(
		object_->transform_->position_ + offset_,
		size_,
		Vector3::Zero_,
		Vector4::White_
	);
}

void BoxCollider::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("BoxCollider")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::Checkbox(u8"���茋��", &isHit_);
		ImGui::DragFloat3(u8"�T�C�Y", &size_.x, 0.1f);
		ImGui::DragFloat3(u8"�I�t�Z�b�g", &offset_.x, 0.1f);

		ImGui::TreePop();
	}

}


// --- �~���̔���̈�̕`�� ---
void CapsuleCollider::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::Instance().GetDebugRenderer()->DrawCapsule(
		object_->transform_->position_ + offset_,
		radius_,
		height_
	);
}

void CapsuleCollider::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("CapsuleCollider")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::Checkbox(u8"���茋��", &isHit_);
		ImGui::DragFloat(u8"���a", &radius_, 0.1f);
		ImGui::DragFloat(u8"����", &height_, 0.1f);
		ImGui::DragFloat3(u8"�I�t�Z�b�g", &offset_.x, 0.1f);

		ImGui::TreePop();
	}
}


// --- ���̔���̈�̕`�� ---
void SphereCollider::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::Instance().GetDebugRenderer()->DrawSphere(
		object_->transform_->position_ + offset_,
		radius_,
		Vector4::White_
	);
}

void SphereCollider::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("SphereCollider")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::Checkbox(u8"���茋��", &isHit_);
		ImGui::DragFloat(u8"���a", &radius_, 0.1f);
		ImGui::DragFloat3(u8"�I�t�Z�b�g", &offset_.x, 0.1f);

		ImGui::TreePop();
	}
}



// --- �|�C���g���C�g�̍X�V���� ---
void PointLightComponent::Update()
{
	if (!isOn_)
		return;

	Graphics::Instance().GetLightingManager()->pointLights_.emplace_back(
		object_->transform_->position_,
		intensity_,
		color_,
		range_
	);
}


// --- �|�C���g���C�g�̃f�o�b�O���`�� ---
void PointLightComponent::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	Graphics::Instance().GetDebugRenderer()->DrawSphere(
		object_->transform_->position_,
		0.5f,
		Vector4::White_
	);
}

void PointLightComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("PointLight")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		ImGui::Checkbox(u8"�L����", &isOn_);
		ImGui::ColorEdit3(u8"�F", &color_.x);
		ImGui::DragFloat(u8"�͈�", &range_, 0.1f, 0.0f);
		ImGui::DragFloat(u8"�Ɠx", &intensity_, 0.1f, 0.0f);

		ImGui::TreePop();
	}
}


// --- �J�����̍X�V���� ---
void CameraComponent::Update()
{
	//Constants::Scene data;
	//Graphics& graphics = Graphics::instance();

	//data.viewProjection_	= viewProjection_;
	//data.invViewProjection_ = invViewProjection_;
	//data.cameraPosition_ = object_->transform_->position_;

	//graphics.updateConstantBuffer<Constants::Scene>(Constants::SCENE, data);

	//graphics.getDeviceContext()->VSSetConstantBuffers(
	//	Constants::SCENE, 1, graphics.constantBuffers[Constants::SCENE].GetAddressOf()
	//);

	//graphics.getDeviceContext()->PSSetConstantBuffers(
	//	Constants::SCENE, 1, graphics.constantBuffers[Constants::SCENE].GetAddressOf()
	//);
}



// --- �J�����̃f�o�b�O�`�� ---
void CameraComponent::Draw(ID3D11DeviceContext* dc)
{
	if (!isVisible_)
		return;

	//Graphics::Instance().getDebugRenderer()->drawCube(
	//	object_->transform_->position_
	//);
}


// --- �J�����̃f�o�b�OGui�X�V ---
void CameraComponent::UpdateDebugGui(float elapsedTime)
{
	ImGui::Spacing();
	ImGui::Separator();
	if (ImGui::TreeNode("Camera")) {
		ImGui::SameLine();
		ImGui::Text("          ");
		ImGui::SameLine();
		ImGui::Checkbox(u8"����", &isVisible_);
		ImGui::Spacing();

		//if (ImGui::Button(u8"�K�p", ImVec2(60, 30)))
		//	SceneManager::instance().scene->camera_ = this->object_;

		ImGui::DragFloat3(u8"�ڕW", &target_.x, 0.1f);
		ImGui::DragFloat(u8"����", &range_, 0.1f);
		ImGui::DragFloat(u8"����p", &fov_, 0.1f);
		ImGui::DragFloat(u8"�ŋߋ���", &nearZ_, 0.1f);
		ImGui::DragFloat(u8"�ŉ�����", &farZ_, 0.1f);

		ImGui::TreePop();
	}
}


// ===== �Q�[���I�u�W�F�N�g�N���X =============================================================================================================================

// --- �R���X�g���N�^ ---
GameObject::GameObject() :
	state_(),
	timer_(),
	isChoose_(),
	type_(ObjectType::NONE),
	transform_(std::make_shared<Transform>())
{
	transform_->SetObject(this);
	components_.emplace_back(transform_);

	static int objectCount = 0;
	name_ = "Object" + std::to_string(objectCount++);
}


// --- �X�V���� ---
void GameObject::Update(float elapsedTime)
{
	if (behavior_)
		behavior_->Execute(this, elapsedTime);

	if (eraser_)
		eraser_->Execute(this);
}


void GameObject::Destroy()
{
	GameObjectManager::Instance().discardList_.emplace(this);
}


// ===== �Q�[���I�u�W�F�N�g�Ǘ��N���X =============================================================================================================================

// --- �X�V���� ---
void GameObjectManager::Update(float elapsedTime)
{
	for (auto& obj : objectList_)
	{
		obj->Update(elapsedTime);

		obj->transform_->MakeTransform();
	}
}


// --- �`�揈�� ---
void GameObjectManager::Draw(ID3D11DeviceContext* dc)
{
	// --- ���X�g�̃\�[�g ---
	if (isSort_)
	{
		objectList_.sort(
			[](const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2)
			{ return obj1->transform_->position_.z > obj2->transform_->position_.z; }
		);
	}

	for (const auto& obj : objectList_)
	{
		for (const auto& comp : obj->components_)
		{
			comp->Draw(dc);
		}
	}
}


// --- �����蔻��̏��� ---
void GameObjectManager::JudgeCollision(float elapsedTime)
{
	for (auto& obj : objectList_)
	{
		if (!obj->behavior_)
			continue;

		for (auto& obj2 : objectList_)
		{
			if (obj == obj2)
				continue;

			if (!obj2->behavior_)
				continue;

			for (auto& collider : obj->colliders_)
			{
				for (auto& collider2 : obj2->colliders_)
				{
					switch (collider->collisionType_)
					{

					case ColliderComponent::CollisionType::BOX2D:
					{
						// --- ��`���m�̏Փ˔��� ---
						if (collider2->collisionType_ != ColliderComponent::CollisionType::BOX2D)
							break;

						BoxCollider2D* c1 = obj->GetComponent<BoxCollider2D>();
						BoxCollider2D* c2 = obj2->GetComponent<BoxCollider2D>();

						//if (Collision::intersectRectangles(
						//	obj->transform_->position_.xy() + c1->offset_, c1->size_,
						//	obj2->transform_->position_.xy() + c2->offset_, c2->size_))
						//{
						//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						//}
						break;
					}

					case ColliderComponent::CollisionType::CIRCLE:
					{
						// --- �~���m�̏Փ˔��� ---
						if (collider2->collisionType_ != ColliderComponent::CollisionType::CIRCLE)
							break;

						CircleCollider* c1 = obj->GetComponent<CircleCollider>();
						CircleCollider* c2 = obj2->GetComponent<CircleCollider>();

						//if (Collision::intersectCircles(
						//	obj->transform_->position_.xy() + c1->offset_, c1->radius_,
						//	obj2->transform_->position_.xy() + c2->offset_, c2->radius_))
						//{
						//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						//}
						break;
					}

					case ColliderComponent::CollisionType::BOX:
					{
						// --- AABB���m�̏Փ˔��� ---
						if (collider2->collisionType_ == ColliderComponent::CollisionType::BOX)
						{
							BoxCollider* c1 = obj->GetComponent<BoxCollider>();
							BoxCollider* c2 = obj2->GetComponent<BoxCollider>();

							//if (Collision::intersectBoxes(
							//	obj->transform_->position_ + c1->offset_, c1->size_,
							//	obj2->transform_->position_ + c2->offset_, c2->size_))
							//{
							//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
							//}
						}

						// --- ����AABB�̏Փ˔��� ---
						else if (collider2->collisionType_ == ColliderComponent::CollisionType::SPHERE)
						{
							BoxCollider* box = obj->GetComponent<BoxCollider>();
							SphereCollider* sphere = obj2->GetComponent<SphereCollider>();

							//if (Collision::intersectSphereAndAABB(
							//	obj2->transform_->position_ + sphere->offset_, sphere->radius_,
							//	obj->transform_->position_ + box->offset_, box->size_))
							//{
							//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
							//}
						}
						break;
					}


					case ColliderComponent::CollisionType::CAPSULE:
					{
						// --- �J�v�Z�����m�̏Փ˔��� ---
						if (collider2->collisionType_ != ColliderComponent::CollisionType::CAPSULE)
							break;

						CapsuleCollider* c1 = obj->GetComponent<CapsuleCollider>();
						CapsuleCollider* c2 = obj2->GetComponent<CapsuleCollider>();

						//if (Collision::intersectCapsules(
						//	obj->transform_->position_ + c1->offset_, c1->radius_, c1->height_,
						//	obj2->transform_->position_ + c2->offset_, c2->radius_, c2->height_))
						//{
						//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
						//}
						break;
					}


					case ColliderComponent::CollisionType::SPHERE:
					{
						// --- �����m�̏Փ˔��� ---
						if (collider2->collisionType_ == ColliderComponent::CollisionType::SPHERE)
						{
							SphereCollider* c1 = obj->GetComponent<SphereCollider>();
							SphereCollider* c2 = obj2->GetComponent<SphereCollider>();

							//if (Collision::intersectSpheres(
							//	obj->transform_->position_ + c1->offset_, c1->radius_,
							//	obj2->transform_->position_ + c2->offset_, c2->radius_))
							//{
							//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
							//}
						}

						// --- ����AABB�̏Փ˔��� ---
						else if (collider2->collisionType_ == ColliderComponent::CollisionType::BOX)
						{
							BoxCollider* box = obj2->GetComponent<BoxCollider>();
							SphereCollider* sphere = obj->GetComponent<SphereCollider>();

							//if (Collision::intersectSphereAndAABB(
							//	obj->transform_->position_ + sphere->offset_, sphere->radius_,
							//	obj2->transform_->position_ + box->offset_, box->size_))
							//{
							//	obj->behavior_->hit(obj.get(), obj2.get(), elapsedTime);
							//}
						}

						break;
					}

					}
				}
			}
		}
	}

}


// --- �I�u�W�F�N�g�̍폜 ---
void GameObjectManager::Remove()
{
	for (GameObject* object : discardList_)
	{
		auto it = std::find_if(
			objectList_.begin(),
			objectList_.end(),
			[object](const std::shared_ptr<GameObject>& ptr) {return ptr.get() == object; }
		);

		if (it != objectList_.end())
			objectList_.erase(it);
	}

	discardList_.clear();
}


// --- �f�o�b�OGui�̍X�V ---
void GameObjectManager::UpdateDebugGui(float elapsedTime)
{
	// --- �ʒu�ƃT�C�Y���Œ� ---
	//ImGui::SetNextWindowPos(ImVec2(1550, 10), ImGuiSetCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(350, 1020), ImGuiSetCond_Always);
	ImGui::SetNextWindowPos(ImVec2(890, 10), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(390, 710), ImGuiSetCond_Always);

	ImGui::Begin("Inspector");

	for (auto& obj : objectList_)
	{
		if (obj->isChoose_)
		{
			ImGui::Text(u8"�I�𒆂̃I�u�W�F�N�g :");
			ImGui::SameLine();
			ImGui::Text(obj->name_.c_str());

			ImGui::Spacing();
			ImGui::InputInt(u8"���", &obj->state_);

			for (auto& component : obj->components_)
			{
				ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
				component->UpdateDebugGui(elapsedTime);
			}
		}
	}

	ImGui::End();
}


// --- �I�u�W�F�N�g���X�g�̕\�� ---
void GameObjectManager::ShowDebugList()
{
	// --- �ʒu�ƃT�C�Y���Œ� ---
	//ImGui::SetNextWindowPos(ImVec2(1340, 10), ImGuiSetCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(200, 1020), ImGuiSetCond_Always);
	ImGui::SetNextWindowPos(ImVec2(690, 10), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(200, 710), ImGuiSetCond_Always);

	ImGui::Begin("Hierarchy");

	for (auto& obj : objectList_)
	{
		if (obj->name_.empty())
			continue;

		if (ImGui::Selectable(obj->name_.c_str(), &obj->isChoose_))
		{
			for (auto& it : objectList_)
				it->isChoose_ = false;

			obj->isChoose_ = true;
		}
	}

	ImGui::End();

}


// --- �I�u�W�F�N�g�̒ǉ� ---
GameObject* GameObjectManager::Add(std::shared_ptr<GameObject> object, const Vector3& position, Behavior* behavior)
{
	object->transform_->position_ = position;
	object->behavior_ = behavior;

	objectList_.emplace_back(object);

	return object.get();
}
