#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include <d3d11.h>
#include <wrl.h>


struct Texture
{
	Texture() {};
	Texture(const Texture& texture) : srv_(texture.srv_), width_(texture.width_), height_(texture.height_) {}
	Texture(ID3D11Device* device, const wchar_t* fileName);
	Texture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv, float width, float height)
		: srv_(srv), width_(width), height_(height) {}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
	float width_;
	float height_;
};


class TextureManager
{
private:
	TextureManager() {}
	~TextureManager() {}

public:
	static TextureManager& Instance()
	{
		static TextureManager instance;
		return instance;
	}

	// --- リソースの取得 ---
	const std::unordered_map<std::wstring, std::shared_ptr<Texture>>& GetResource() { return resources_; }

	// --- テクスチャの読み込み ---
	Texture* LoadTexture(ID3D11Device* device, const wchar_t* fileName, std::wstring* path = nullptr);

	// --- テクスチャの取得 ---
	Texture* GetTexture(const wchar_t* fileName, std::wstring* path = nullptr);

	// --- デバッグGuiの更新 ---
	void UpdateDebugGui();
	
private:
	std::unordered_map<std::wstring, std::shared_ptr<Texture>> resources_;
};

