#include "Constant.hlsli"

#include "PostEffect.hlsli"


SamplerState samplerStates[3] : register(s0);
Texture2D sceneTexture : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[ANISOTROPIC], pin.texcoord_);
	
	// --- �T���v�����O�����J���[�̖��邳���v�Z ---
    float t = dot(color.xyz, float3(0.2125, 0.7154, 0.0721));
	
	// --- �l���}�C�i�X�ɂȂ�Əo�͂���Ȃ� ---
	//clip(t - 1.0);
	
	return color;
}