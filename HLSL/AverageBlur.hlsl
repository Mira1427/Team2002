#include "Constant.hlsli"

#include "PostEffect.hlsli"


SamplerState samplerStates[3] : register(s0);
Texture2D    sceneTexture : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_);
	
	// --- 基準テクセル + 近傍8テクセルの平均を計算する ---
    float offsetU = 1.5 / scene_.windowSize_.x;
    float offsetV = 1.5 / scene_.windowSize_.y;
    
    // --- 基準テクセルから右のテクセルカラーをサンプリング ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(offsetU, 0.0));
    
    // --- 左から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(-offsetU, 0.0));
    
    // --- 下から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(0.0, offsetV));
    
    // --- 上から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(0.0, -offsetV));
    
    // --- 右下から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(offsetU, offsetV));
    
    // --- 右上から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(offsetU, -offsetV));
    
    // --- 左下から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(-offsetU, offsetV));
    
    // --- 左上から ---
    color += sceneTexture.Sample(samplerStates[LINEAR], pin.texcoord_ + float2(-offsetU, -offsetV));
    
    
    // --- 基準テクセルと近傍８テクセルの平均を計算 ---
    color /= 9.0;
    
    return color;
}