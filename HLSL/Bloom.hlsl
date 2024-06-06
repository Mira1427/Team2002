#include "Constant.hlsli"

#include "PostEffect.hlsli"


#define SCENE 0
#define BLUR1 1
#define BLUR2 2
#define BLUR3 3
#define BLUR4 4

SamplerState samplerStates[3] : register(s0);
Texture2D textures[5] : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{
    float4 scene = textures[SCENE].Sample(samplerStates[LINEAR], pin.texcoord_);
    float4 blur = textures[BLUR1].Sample(samplerStates[LINEAR], pin.texcoord_);
    blur += textures[BLUR2].Sample(samplerStates[LINEAR], pin.texcoord_);
    blur += textures[BLUR3].Sample(samplerStates[LINEAR], pin.texcoord_);
    blur += textures[BLUR4].Sample(samplerStates[LINEAR], pin.texcoord_);
    
    blur /= 4.0;
    blur.a = 1.0;
    
    scene.xyz += blur.xyz * 1.25;
    
    
#if 1
    const float exposure = 1.2;
    scene.rgb = 1 - exp(-scene.rgb * exposure);

    scene.rgb = pow(scene.rgb, 1.0 / 2.2);
#endif
	
	return scene;
}