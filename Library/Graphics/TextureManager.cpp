#include "TextureManager.h"

#include "../../External/ImGui/imgui.h"

#include "../Library/Library.h"

#include "Shader.h"

Texture::Texture(ID3D11Device* device, const wchar_t* fileName)
{

	D3D11_TEXTURE2D_DESC tex2dDesc = {};
	Shader::LoadTextureFromFile(device, fileName, srv_.GetAddressOf(), &tex2dDesc);

	width_ = static_cast<float>(tex2dDesc.Width);
	height_ = static_cast<float>(tex2dDesc.Height);

}


// --- テクスチャの読み込み ---
Texture* TextureManager::LoadTexture(ID3D11Device* device, const wchar_t* fileName, std::wstring* path)
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
		std::shared_ptr<Texture> texture;
		texture = std::make_shared<Texture>(device, fileName);	// テクスチャの作成
		resources_.insert(std::make_pair(fileName, texture));	// リソースに追加

		if (path)
			*path = fileName;

		return texture.get();
	}
}


// --- テクスチャの取得 ---
Texture* TextureManager::GetTexture(const wchar_t* fileName, std::wstring* path)
{
	return LoadTexture(RootsLib::DX11::GetDevice(), fileName, path);
}



// --- デバッグGuiの更新 ---
void TextureManager::UpdateDebugGui()
{
#ifdef USE_IMGUI
	for (auto& it : resources_)
	{
		std::string str = RootsLib::String::ConvertWideChar(it.first.c_str());
		ImGui::Text(str.c_str());
		ImGui::Text(u8"幅 : %f 高さ : %f", it.second->width_, it.second->height_);
		ImGui::Image(it.second->srv_.Get(), { it.second->width_ * 0.05f, it.second->height_ * 0.05f });
		ImGui::Spacing();
		ImGui::Separator();
	}
#endif
}
