#include "SpriteRenderer.hlsli"

SamplerState samplerStates[3] : register(s0);
texture2D videoTexture : register(t0);

float3 YUVToRGB(float3 yuv)
{
    static const float3 yuvCoefR = { 1.164, 0.000, 1.596 };
    static const float3 yuvCoefG = { 1.164, -0.392, -0.813 };
    static const float3 yuvCoefB = { 1.164, 2.017, 0.000 };
    yuv -= float3(0.0625, 0.5, 0.5);
    
    return saturate(
        float3(
            dot(yuv, yuvCoefR),
            dot(yuv, yuvCoefG),
            dot(yuv, yuvCoefB)
        )
    );
}


float4 main(VSOutput pin) : SV_TARGET
{
    float y = videoTexture.Sample(samplerStates[LINEAR], float2(pin.texcoord_.x, pin.texcoord_.y * 0.5)).r;
    float u = videoTexture.Sample(samplerStates[LINEAR], float2(pin.texcoord_.x * 0.5, 0.50 + pin.texcoord_.y * 0.25)).r;
    float v = videoTexture.Sample(samplerStates[LINEAR], float2(pin.texcoord_.x * 0.5, 0.75 + pin.texcoord_.y * 0.25)).r;
    return float4(YUVToRGB(float3(y, u, v)), 1.f);
}