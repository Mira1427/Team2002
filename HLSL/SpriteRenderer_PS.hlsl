#include "./SpriteRenderer.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D mainTexture : register(t0);


float4 main(VSOutput vout) : SV_TARGET
{
	float4 color = mainTexture.Sample(samplerStates[ANISOTROPIC], vout.texcoord_);

	// --- ãtÉKÉìÉ}ï‚ê≥ ---
	color.rgb = pow(color.rgb, scene_.invGamma_);

	return color * vout.color_;
}