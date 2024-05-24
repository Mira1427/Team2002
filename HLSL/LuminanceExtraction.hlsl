#include "Constant.hlsli"

#include "PostEffect.hlsli"


SamplerState samplerStates[3] : register(s0);
Texture2D sceneTexture : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[ANISOTROPIC], pin.texcoord_);
	
	// --- サンプリングしたカラーの明るさを計算 ---
    float t = dot(color.xyz, float3(0.2125, 0.7154, 0.0721));
	
	// --- 値がマイナスになると出力されない ---
	//clip(t - 1.0);
	
	return color;
}