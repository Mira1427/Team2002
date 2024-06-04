#include "MeshRenderer.hlsli"

#include "Common.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[6] : register(t0);

SamplerComparisonState comparisonSamplerState : register(s4);
Texture2D shadowMap : register(t8);


float4 main(VSOutput pin) : SV_TARGET
{
	// --- 法線マップから法線を取得 ---
    float3 normal = CalcNormalFromMap(
        pin.worldNormal,
        pin.worldTangent,
        textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord)
    );
    
    
    // --- アルベドカラー ( 拡散反射光 ) ---
    float4 albedoColor = textureMaps[ALBEDO_MAP].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    float alpha = albedoColor.a;
    
    // --- スペキュラーカラーはアルベドカラーと同じに ---
    float3 specColor = albedoColor;
    
    // --- 金属度 ---
    float metaric = textureMaps[METARIC_MAP].Sample(samplerStates[ANISOTROPIC], pin.texcoord).r;
    
    // --- 粗さ ---
    float roughness = textureMaps[ROUGHNESS_MAP].Sample(samplerStates[ANISOTROPIC], pin.texcoord).r;
    
    
    // --- 視線に向かって伸びるベクトルの計算 ---
    float3 toEye = scene_.cameraPosition_.xyz - pin.worldPosition.xyz;
    
    
    float3 lig = 0;
    
    
    // --- フレネル反射を考慮した拡散反射を計算 ---
    float diffuseFromFresnel = CalcDiffuseFromFresnel(
        normal,
        -scene_.directionLight_.direction_,
        toEye
    );
    
    // --- 正規化Lambert拡散反射を求める ---
    float3 dotNL = saturate(dot(normal, -scene_.directionLight_.direction_));
    float3 lambertDiffuse = scene_.directionLight_.color_ * scene_.directionLight_.intensity_ * dotNL / PI;
    
    // --- 最終的な拡散反射光を計算する ---
    float3 diffuse = albedoColor.xyz * diffuseFromFresnel * lambertDiffuse;
    
    
    // --- Cook-Torranceモデルの鏡面反射率を計算する ---
    float3 spec = CalcCookTorranceSpecular(
        -scene_.directionLight_.direction_,
        toEye,
        normal,
        metaric
    ) * scene_.directionLight_.color_ * scene_.directionLight_.intensity_;
    
    // --- スペキュラーカラーの強さを鏡面反射率として扱う ---
    spec *= lerp(float3(1.0, 1.0, 1.0), specColor, metaric);
    
    
    // --- ラフネスを使って拡散反射光と鏡面反射光を合成する ---
    lig += diffuse * (1.0 - roughness) + spec;
    
    
    // ===== ポイントライトの処理 =============================================================================================================================

    float3 diffPoint = float3(0.0f, 0.0f, 0.0f);
    float3 specPoint = float3(0.0f, 0.0f, 0.0f);

	[unroll]
    for (int i = 0; i < POINT_LIGHT_MAX; i++)
    {
        float3 ligDir = pin.worldPosition.xyz - scene_.pointLights_[i].position_;
        ligDir = normalize(ligDir);

        float3 ligColor = mul(scene_.pointLights_[i].color_, scene_.pointLights_[i].intensity_);

        float3 diff = calcLambertDiffuse(
			ligDir,
			ligColor,
			normal
		);

        float3 spec = calcPhongSpecular(
			ligDir,
			ligColor,
			scene_.cameraPosition_.xyz,
			pin.worldPosition.xyz,
			normal
		);

        float distance = length(pin.worldPosition.xyz - scene_.pointLights_[i].position_);

        float affect = 1.0f - 1.0f / scene_.pointLights_[i].range_ * distance;

        affect = max(0.0f, affect);

        affect = pow(affect, 3.0f);

        diff *= affect;
        spec *= affect;

        diffPoint += diff;
        specPoint += spec;
    }

   
    // --- 環境光による底上げ ---
    lig += scene_.ambientLight_.color_ * albedoColor.xyz;
    
    lig += diffPoint + specPoint;
  
    float4 finalColor = 1.0;
    finalColor.xyz = lig; 
    
    
    // --- エミッシブ ---
    float4 emissiveColor = textureMaps[EMISSIVE_MAP].Sample(samplerStates[LINEAR], pin.texcoord);
    finalColor.xyz += emissiveColor * 3.0;
    
    	
	
	// --- シャドウマップ ---
    const float shadowDepthBias = max(0.01 * (1.0 - dot(normal, normalize(-scene_.directionLight_.direction_.xyz))), 0.001);
    
    float4 lightViewPosition = mul(pin.worldPosition, scene_.directionLight_.viewProjection_);
    lightViewPosition = lightViewPosition / lightViewPosition.w;
    float2 lightViewTexCoord = 0;
	
    lightViewTexCoord.x = lightViewPosition.x * +0.5 + 0.5;
    lightViewTexCoord.y = lightViewPosition.y * -0.5 + 0.5;
    float depth = saturate(lightViewPosition.z - shadowDepthBias);
	
    float3 shadowFactor = 1.0;
    shadowFactor = shadowMap.SampleCmpLevelZero(comparisonSamplerState, lightViewTexCoord, depth).xxx;
    
    shadowFactor = lerp(0.4, 1.0, shadowFactor);
    
    finalColor.xyz *= shadowFactor;
    finalColor.a = alpha;


    return finalColor * pin.color;
}