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


// --- �e�N�X�`���̓ǂݍ��� ---
Texture* TextureManager::LoadTexture(ID3D11Device* device, const wchar_t* fileName, std::wstring* path)
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
		std::shared_ptr<Texture> texture;
		texture = std::make_shared<Texture>(device, fileName);	// �e�N�X�`���̍쐬
		resources_.insert(std::make_pair(fileName, texture));	// ���\�[�X�ɒǉ�

		if (path)
			*path = fileName;

		return texture.get();
	}
}


// --- �e�N�X�`���̎擾 ---
Texture* TextureManager::GetTexture(const wchar_t* fileName, std::wstring* path)
{
	return LoadTexture(RootsLib::DX11::GetDevice(), fileName, path);
}



// --- �f�o�b�OGui�̍X�V ---
void TextureManager::UpdateDebugGui()
{
#ifdef USE_IMGUI
	for (auto& it : resources_)
	{
		std::string str = RootsLib::String::ConvertWideChar(it.first.c_str());
		ImGui::Text(str.c_str());
		ImGui::Text(u8"�� : %f ���� : %f", it.second->width_, it.second->height_);
		ImGui::Image(it.second->srv_.Get(), { it.second->width_ * 0.05f, it.second->height_ * 0.05f });
		ImGui::Spacing();
		ImGui::Separator();
	}
#endif
}
