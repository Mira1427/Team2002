#include "MeshRenderer.hlsli"

#include "Common.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);

SamplerComparisonState comparisonSamplerState : register(s4);
Texture2D shadowMap : register(t8);


float4 main(VSOutput pin) : SV_TARGET
{		
	// --- 法線マップから法線を取得 --- 
    float3 N = CalcNormalFromMap(
        pin.worldNormal,
        pin.worldTangent,
        textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord)
    );


	// ===== ディレクションライトの処理 ========================================================================================================================

	// --- Lambert拡散反射光の計算 ---
	float3 diffDirection = calcLambertDiffuse(
		scene_.directionLight_.direction_,
		mul(scene_.directionLight_.color_, scene_.directionLight_.intensity_),
		N
	);

	// --- Phong鏡面反射光の計算 ---
	float3 specDirection = calcPhongSpecular(
		scene_.directionLight_.direction_,
		mul(scene_.directionLight_.color_, scene_.directionLight_.intensity_),
		scene_.cameraPosition_.xyz,
		pin.worldPosition.xyz,
		N
	);


	// ===== ポイントライトの処理 =============================================================================================================================

	float3 diffPoint = float3(0.0f, 0.0f, 0.0f);
	float3 specPoint = float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for (int i = 0; i < POINT_LIGHT_MAX; i++)
	{
        float3 ligDir = pin.worldPosition.xyz - scene_.pointLights_[i].position_;
		ligDir = normalize(ligDir);

        float3 ligColor = mul(scene_.pointLights_[i].color_, scene_.pointLights_[i].intensity_);

		float3 diffuse = calcLambertDiffuse(
			ligDir,
			ligColor,
			N
		);

		float3 specular = calcPhongSpecular(
			ligDir,
			ligColor,
			scene_.cameraPosition_.xyz,
			pin.worldPosition.xyz,
			N
		);

        float distance = length(pin.worldPosition.xyz - scene_.pointLights_[i].position_);

        float affect = 1.0f - 1.0f / scene_.pointLights_[i].range_ * distance;

		affect = max(0.0f, affect);

		affect = pow(affect, 3.0f);

		diffuse *= affect;
		specular *= affect;

		diffPoint += diffuse;
		specPoint += specular;
	}

	// --- 最終的なライトの色 ---
    float3 light = diffDirection + specDirection + diffPoint + specPoint + scene_.ambientLight_.color_;

	// --- カラーマップの取得 ---
	float4 color = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord);

	// --- 逆ガンマ補正 ---
	color.rgb = pow(color.rgb, scene_.invGamma_);

	// --- ライトの色を乗算 ---
	color.xyz *= light;

	float4 emissive = textureMaps[2].Sample(samplerStates[LINEAR], pin.texcoord);
	emissive *= 3.0;

	color.xyz += emissive.xyz;
	
    
    	
	
	// --- シャドウマップ ---
    const float shadowDepthBias = max(0.01 * (1.0 - dot(N, normalize(-scene_.directionLight_.direction_.xyz))), 0.001);
    
    float4 lightViewPosition = mul(pin.worldPosition, scene_.directionLight_.viewProjection_);
    lightViewPosition = lightViewPosition / lightViewPosition.w;
    float2 lightViewTexCoord = 0;
	
    lightViewTexCoord.x = lightViewPosition.x * +0.5 + 0.5;
    lightViewTexCoord.y = lightViewPosition.y * -0.5 + 0.5;
    float depth = saturate(lightViewPosition.z - shadowDepthBias);
	
    float3 shadowFactor = 1.0;
    shadowFactor = shadowMap.SampleCmpLevelZero(comparisonSamplerState, lightViewTexCoord, depth).xxx;
    
    shadowFactor = lerp(0.4, 1.0, shadowFactor);
    
    color.xyz *= shadowFactor;

	return color * pin.color;
}