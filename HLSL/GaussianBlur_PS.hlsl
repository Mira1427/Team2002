#include "Constant.hlsli"

#include "PostEffect.hlsli"


SamplerState samplerStates[3] : register(s0);
Texture2D sceneTexture : register(t0);


float4 main(PSBlurInput pin) : SV_TARGET
{
    float4 color;
    
    // --- 基準テクセルからプラス方向に8テクセル、重み付きでサンプリング ---
    color  = blur_.weights[0].x * sceneTexture.Sample(samplerStates[LINEAR], pin.tex0_.xy);
    color += blur_.weights[0].y * sceneTexture.Sample(samplerStates[LINEAR], pin.tex1_.xy);
    color += blur_.weights[0].z * sceneTexture.Sample(samplerStates[LINEAR], pin.tex2_.xy);
    color += blur_.weights[0].w * sceneTexture.Sample(samplerStates[LINEAR], pin.tex3_.xy);
    color += blur_.weights[1].x * sceneTexture.Sample(samplerStates[LINEAR], pin.tex4_.xy);
    color += blur_.weights[1].y * sceneTexture.Sample(samplerStates[LINEAR], pin.tex5_.xy);
    color += blur_.weights[1].z * sceneTexture.Sample(samplerStates[LINEAR], pin.tex6_.xy);
    color += blur_.weights[1].w * sceneTexture.Sample(samplerStates[LINEAR], pin.tex7_.xy);
    
    // --- 基準テクセルからマイナス方向に8テクセル、重み付きでサンプリング ---
    color  = blur_.weights[0].x * sceneTexture.Sample(samplerStates[LINEAR], pin.tex0_.zw);
    color += blur_.weights[0].y * sceneTexture.Sample(samplerStates[LINEAR], pin.tex1_.zw);
    color += blur_.weights[0].z * sceneTexture.Sample(samplerStates[LINEAR], pin.tex2_.zw);
    color += blur_.weights[0].w * sceneTexture.Sample(samplerStates[LINEAR], pin.tex3_.zw);
    color += blur_.weights[1].x * sceneTexture.Sample(samplerStates[LINEAR], pin.tex4_.zw);
    color += blur_.weights[1].y * sceneTexture.Sample(samplerStates[LINEAR], pin.tex5_.zw);
    color += blur_.weights[1].z * sceneTexture.Sample(samplerStates[LINEAR], pin.tex6_.zw);
    color += blur_.weights[1].w * sceneTexture.Sample(samplerStates[LINEAR], pin.tex7_.zw);
	
	return float4(color.xyz, 1.0f);
}