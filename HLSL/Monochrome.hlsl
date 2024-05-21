#include "Constant.hlsli"

#include "PostEffect.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D    sceneTexture : register(t0);

float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[POINT], pin.texcoord_);
	
    float Y = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
    color.r *= Y;
    color.g *= Y;
    color.b *= Y;
	
	return color * pin.color_;
}