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


// --- ���f���̎擾 ---
MeshRenderer* ModelManager::GetModel(const char* fileName, std::string* path, bool triangulate, bool isRayCast)
{
	return LoadModel(RootsLib::DX11::GetDevice(), fileName, triangulate, isRayCast, path);
}


// --- �f�o�b�OGui�̍X�V ---
void ModelManager::UpdateDebugGui()
{
#ifdef USE_IMGUI
	for (auto& it : resources_)
	{
		// --- �t�@�C���p�X�̕\�� ---
		if (ImGui::TreeNode(it.first.c_str()))
		{
			// --- �e�N�X�`���̎�ނ̔z�� ---
			static const char* materialTypes[static_cast<size_t>(MaterialLabel::MAX)] =
			{
				u8"�J���[", u8"�@��", u8"�G�~�b�V�u", u8"���t�l�X", u8"���^���b�N"
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
#endif
}
