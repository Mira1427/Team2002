#include "Constant.hlsli"

#include "PostEffect.hlsli"


SamplerState samplerStates[3] : register(s0);
Texture2D    sceneTexture : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_);
	
	// --- ��e�N�Z�� + �ߖT8�e�N�Z���̕��ς��v�Z���� ---
    float offsetU = 1.5 / scene_.windowSize_.x;
    float offsetV = 1.5 / scene_.windowSize_.y;
    
    // --- ��e�N�Z������E�̃e�N�Z���J���[���T���v�����O ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(offsetU, 0.0));
    
    // --- ������ ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(-offsetU, 0.0));
    
    // --- ������ ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(0.0, offsetV));
    
    // --- �ォ�� ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(0.0, -offsetV));
    
    // --- �E������ ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(offsetU, offsetV));
    
    // --- �E�ォ�� ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(offsetU, -offsetV));
    
    // --- �������� ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(-offsetU, offsetV));
    
    // --- ���ォ�� ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(-offsetU, -offsetV));
    
    
    // --- ��e�N�Z���ƋߖT�W�e�N�Z���̕��ς��v�Z ---
    color /= 9.0;
    
    return color;
}