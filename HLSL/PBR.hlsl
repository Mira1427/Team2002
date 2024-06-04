#include "MeshRenderer.hlsli"

#include "Common.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[6] : register(t0);

SamplerComparisonState comparisonSamplerState : register(s4);
Texture2D shadowMap : register(t8);


float4 main(VSOutput pin) : SV_TARGET
{
	// --- �@���}�b�v����@�����擾 ---
    float3 normal = CalcNormalFromMap(
        pin.worldNormal,
        pin.worldTangent,
        textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord)
    );
    
    
    // --- �A���x�h�J���[ ( �g�U���ˌ� ) ---
    float4 albedoColor = textureMaps[ALBEDO_MAP].Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    float alpha = albedoColor.a;
    
    // --- �X�y�L�����[�J���[�̓A���x�h�J���[�Ɠ����� ---
    float3 specColor = albedoColor;
    
    // --- �����x ---
    float metaric = textureMaps[METARIC_MAP].Sample(samplerStates[ANISOTROPIC], pin.texcoord).r;
    
    // --- �e�� ---
    float roughness = textureMaps[ROUGHNESS_MAP].Sample(samplerStates[ANISOTROPIC], pin.texcoord).r;
    
    
    // --- �����Ɍ������ĐL�т�x�N�g���̌v�Z ---
    float3 toEye = scene_.cameraPosition_.xyz - pin.worldPosition.xyz;
    
    
    float3 lig = 0;
    
    
    // --- �t���l�����˂��l�������g�U���˂��v�Z ---
    float diffuseFromFresnel = CalcDiffuseFromFresnel(
        normal,
        -scene_.directionLight_.direction_,
        toEye
    );
    
    // --- ���K��Lambert�g�U���˂����߂� ---
    float3 dotNL = saturate(dot(normal, -scene_.directionLight_.direction_));
    float3 lambertDiffuse = scene_.directionLight_.color_ * scene_.directionLight_.intensity_ * dotNL / PI;
    
    // --- �ŏI�I�Ȋg�U���ˌ����v�Z���� ---
    float3 diffuse = albedoColor.xyz * diffuseFromFresnel * lambertDiffuse;
    
    
    // --- Cook-Torrance���f���̋��ʔ��˗����v�Z���� ---
    float3 spec = CalcCookTorranceSpecular(
        -scene_.directionLight_.direction_,
        toEye,
        normal,
        metaric
    ) * scene_.directionLight_.color_ * scene_.directionLight_.intensity_;
    
    // --- �X�y�L�����[�J���[�̋��������ʔ��˗��Ƃ��Ĉ��� ---
    spec *= lerp(float3(1.0, 1.0, 1.0), specColor, metaric);
    
    
    // --- ���t�l�X���g���Ċg�U���ˌ��Ƌ��ʔ��ˌ����������� ---
    lig += diffuse * (1.0 - roughness) + spec;
    
    
    // ===== �|�C���g���C�g�̏��� =============================================================================================================================

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

   
    // --- �����ɂ���グ ---
    lig += scene_.ambientLight_.color_ * albedoColor.xyz;
    
    lig += diffPoint + specPoint;
  
    float4 finalColor = 1.0;
    finalColor.xyz = lig; 
    
    
    // --- �G�~�b�V�u ---
    float4 emissiveColor = textureMaps[EMISSIVE_MAP].Sample(samplerStates[LINEAR], pin.texcoord);
    finalColor.xyz += emissiveColor * 3.0;
    
    	
	
	// --- �V���h�E�}�b�v ---
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