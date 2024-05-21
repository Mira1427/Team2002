#include "SkyMap.h"

#include "../../External/ImGui/imgui.h"

#include "Graphics.h"
#include "Shader.h"
#include "TextureManager.h"

#include "../Library/Library.h"


// --- �R���X�g���N�^ ---
SkyMap::SkyMap(ID3D11Device* device, const wchar_t* fileName)
{
	Graphics& graphics = Graphics::Instance();

	// --- �e�N�X�`���̓ǂݍ��� ---
	D3D11_TEXTURE2D_DESC tex2DDesc{};
	Shader::LoadTextureFromFile(device, fileName, shaderResourceView_.GetAddressOf(), &tex2DDesc);

	if (tex2DDesc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
		isCubeTexture_ = true;

	// --- ���_�V�F�[�_�[�̍쐬 ---
	Shader::CreateVSFromCSO(graphics.GetDevice(), "./Data/Shader/SkyMap_VS.cso", vertexShader_.GetAddressOf());

	// --- �s�N�Z���V�F�[�_�̍쐬 ---
	Shader::CreatePSFromCSO(graphics.GetDevice(), "./Data/Shader/SkyMap_PS.cso", skyMapPS_.GetAddressOf());
	Shader::CreatePSFromCSO(graphics.GetDevice(), "./Data/Shader/SkyBox_PS.cso", skyBoxPS_.GetAddressOf());
}


// --- �`�揈�� ---
void SkyMap::Draw(ID3D11DeviceContext* dc)
{
	RootsLib::Depth::SetState(DepthState::TEST_OFF, DepthState::WRITE_OFF);
	RootsLib::Raster::SetState(RasterState::CULL_BACK);

	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetInputLayout(NULL);
	
	dc->VSSetShader(vertexShader_.Get(), 0, 0);
	dc->PSSetShader(isCubeTexture_ ? skyBoxPS_.Get() : skyMapPS_.Get(), 0, 0);

	dc->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());

	dc->Draw(4, 0);

	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
}


// --- �f�o�b�OGui�̍X�V ---
void SkyMap::OpenTexture()
{
#ifdef USE_IMGUI

	// --- �e�N�X�`���̓ǂݍ��� ---
	shaderResourceView_ = TextureManager::Instance().LoadTexture(RootsLib::DX11::GetDevice(), RootsLib::File::GetFileName().c_str())->srv_;

#endif
}
