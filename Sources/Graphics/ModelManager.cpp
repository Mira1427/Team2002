#include "ModelManager.h"

#include "../../External/ImGui/imgui.h"

#include "TextureManager.h"

#include "../Library/Library.h"


// --- モデルの読み込み ---
MeshRenderer* ModelManager::LoadModel(ID3D11Device* device, const char* fileName, bool triangulate, bool isRayCast, std::string* path)
{
	// --- ファイルパスの検索 ---
	auto it = resources_.find(fileName);

	// --- 見つかったら ---
	if (it != resources_.end())
	{
		auto& texture = it->second;

		if (path)
			*path = fileName;

		return texture.get();
	}

	// --- 見つからなかったら ---
	else
	{
		std::shared_ptr<MeshRenderer> model;
		model = std::make_shared<MeshRenderer>(device, fileName, triangulate, isRayCast);	// モデルの作成
		resources_.insert(std::make_pair(fileName, model));	// リソースに追加

		if (path)
			*path = fileName;

		return model.get();
	}
}


// --- モデルの取得 ---
MeshRenderer* ModelManager::GetModel(const char* fileName, std::string* path, bool triangulate, bool isRayCast)
{
	return LoadModel(RootsLib::DX11::GetDevice(), fileName, triangulate, isRayCast, path);
}


// --- デバッグGuiの更新 ---
void ModelManager::UpdateDebugGui()
{
#ifdef USE_IMGUI
	for (auto& it : resources_)
	{
		// --- ファイルパスの表示 ---
		if (ImGui::TreeNode(it.first.c_str()))
		{
			// --- テクスチャの種類の配列 ---
			static const char* materialTypes[static_cast<size_t>(MaterialLabel::MAX)] =
			{
				u8"カラー", u8"法線", u8"エミッシブ", u8"ラフネス", u8"メタリック"
			};

			// --- マテリアルの表示 ---
			for (auto& material : it.second->materials)
			{
				ImGui::Separator();

				// --- マテリアルの数だけ回す ---
				for (size_t i = 0; i < static_cast<size_t>(MaterialLabel::MAX); i++)
				{
					//ImGui::Separator();
					ImGui::BulletText(materialTypes[i]);
					ImGui::SameLine();
					ImGui::Text(material.second.textureFileNames[i].c_str());
					ImGui::SameLine();

					// --- マテリアルの変更 ---
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
