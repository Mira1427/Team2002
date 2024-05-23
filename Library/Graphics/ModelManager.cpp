#include "ModelManager.h"

#include "../../External/ImGui/imgui.h"

#include "TextureManager.h"

#include "../Library/Library.h"


// --- ���f���̓ǂݍ��� ---
MeshRenderer* ModelManager::LoadModel(ID3D11Device* device, const char* fileName, bool triangulate, bool isRayCast, std::string* path)
{
	// --- �t�@�C���p�X�̌��� ---
	auto it = resources_.find(fileName);

	// --- ���������� ---
	if (it != resources_.end())
	{
		auto& texture = it->second;

		if (path)
			*path = fileName;

		return texture.get();
	}

	// --- ������Ȃ������� ---
	else
	{
		std::shared_ptr<MeshRenderer> model;
		model = std::make_shared<MeshRenderer>(device, fileName, triangulate, isRayCast);	// ���f���̍쐬
		resources_.insert(std::make_pair(fileName, model));	// ���\�[�X�ɒǉ�

		if (path)
			*path = fileName;

		return model.get();
	}
}


// --- �C���X�^���X���b�V���̃��[�h ---
InstancedMesh* ModelManager::LoadInstancedMesh(ID3D11Device* device, const char* fileName, size_t maxInstances, bool triangulate, std::string* path)
{
	// --- �t�@�C���p�X�̌��� ---
	auto it = instancedMeshes_.find(fileName);

	// --- ���������� ---
	if (it != instancedMeshes_.end())
	{
		auto& texture = it->second;

		if (path)
			*path = fileName;

		return texture.get();
	}

	// --- ������Ȃ������� ---
	else
	{
		std::shared_ptr<InstancedMesh> model;
		model = std::make_shared<InstancedMesh>(device, fileName, triangulate, maxInstances);	// ���f���̍쐬
		instancedMeshes_.insert(std::make_pair(fileName, model));	// ���\�[�X�ɒǉ�

		if (path)
			*path = fileName;

		return model.get();
	}
}


// --- ���f���̎擾 ---
MeshRenderer* ModelManager::GetModel(const char* fileName, std::string* path, bool triangulate, bool isRayCast)
{
	return LoadModel(RootsLib::DX11::GetDevice(), fileName, triangulate, isRayCast, path);
}


// --- �C���X�^���X���b�V���̎擾 ---
InstancedMesh* ModelManager::GetInstancedMesh(const char* fileName, size_t maxInstances, std::string* path, bool triangulate)
{
	return LoadInstancedMesh(RootsLib::DX11::GetDevice(), fileName, triangulate, maxInstances, path);
}


// --- �f�o�b�OGui�̍X�V ---
void ModelManager::UpdateDebugGui()
{
#ifdef USE_IMGUI
	// --- �A�j���[�V�����t���̃��b�V���̃f�o�b�O�`�� ---
	if (ImGui::TreeNode(u8"Skeletal Mesh"))
	{
		for (auto& it : resources_)
		{
			// --- �t�@�C���p�X�̕\�� ---
			if (ImGui::TreeNode(it.first.c_str()))
			{
				// --- �e�N�X�`���̎�ނ̔z�� ---
				static const char* materialTypes[static_cast<size_t>(MaterialLabel::MAX)] =
				{
					u8"�J���[", u8"�@��", u8"�G�~�b�V�u", u8"���t�l�X", u8"���^���b�N", u8"�A���r�G���g�I�N���[�W����"
				};

				// --- �}�e���A���̕\�� ---
				for (auto& material : it.second->materials)
				{
					ImGui::Separator();

					// --- �}�e���A���̐������� ---
					for (size_t i = 0; i < static_cast<size_t>(MaterialLabel::MAX); i++)
					{
						//ImGui::Separator();
						ImGui::BulletText(materialTypes[i]);
						ImGui::SameLine();
						ImGui::Text(material.second.textureFileNames[i].c_str());
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

						ImGui::Image(material.second.shaderResourceViews[i].Get(), ImVec2(250.0f, 250.0f));
					}
					ImGui::Spacing();
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}


	// --- �C���X�^���X���b�V���̃f�o�b�O�`�� ---
	if (ImGui::TreeNode(u8"Instanced Mesh"))
	{
		for (auto& it : instancedMeshes_)
		{
			// --- �t�@�C���p�X�̕\�� ---
			if (ImGui::TreeNode(it.first.c_str()))
			{
				// --- �e�N�X�`���̎�ނ̔z�� ---
				static const char* materialTypes[static_cast<size_t>(MaterialLabel::MAX)] =
				{
					u8"�J���[", u8"�@��", u8"�G�~�b�V�u", u8"���t�l�X", u8"���^���b�N", u8"�A���r�G���g�I�N���[�W����"
				};

				// --- �}�e���A���̕\�� ---
				for (auto& material : it.second->materials_)
				{
					ImGui::Separator();

					// --- �}�e���A���̐������� ---
					for (size_t i = 0; i < static_cast<size_t>(MaterialLabel::MAX); i++)
					{
						//ImGui::Separator();
						ImGui::BulletText(materialTypes[i]);
						ImGui::SameLine();
						ImGui::Text(material.second.textureFileNames[i].c_str());
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

						ImGui::Image(material.second.shaderResourceViews[i].Get(), ImVec2(250.0f, 250.0f));
					}
					ImGui::Spacing();
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
#endif
}
