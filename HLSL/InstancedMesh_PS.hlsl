#include "InstancedMesh.hlsli"

#include "./Common.hlsli"

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);


float4 main(VSOutput pin) : SV_TARGET
{
	// --- �@���}�b�v����@�����擾 ---
    float3 N = normalize(pin.worldNormal_.xyz);
    float3 T = normalize(pin.worldTangent_.xyz);
    float sigma = pin.worldTangent_.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
	
    float4 normal = textureMaps[1].Sample(samplerStates[LINEAR], pin.texcoord_);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));


	// ===== �f�B���N�V�������C�g�̏��� ========================================================================================================================

	// --- Lambert�g�U���ˌ��̌v�Z ---
    float3 diffDirection = calcLambertDiffuse(
		scene_.directionLight_.direction_,
		mul(scene_.directionLight_.color_, scene_.directionLight_.intensity_),
		N
	);

	// --- Phong���ʔ��ˌ��̌v�Z ---
    float3 specDirection = calcPhongSpecular(
		scene_.directionLight_.direction_,
		mul(scene_.directionLight_.color_, scene_.directionLight_.intensity_),
		scene_.cameraPosition_.xyz,
		pin.worldPosition_.xyz,
		N
	);


	// ===== �|�C���g���C�g�̏��� =============================================================================================================================

    float3 diffPoint = float3(0.0f, 0.0f, 0.0f);
    float3 specPoint = float3(0.0f, 0.0f, 0.0f);

	[unroll]
    for (int i = 0; i < POINT_LIGHT_MAX; i++)
    {
        float3 ligDir = pin.worldPosition_.xyz - scene_.pointLights_[i].position_;
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
			pin.worldPosition_.xyz,
			N
		);

        float distance = length(pin.worldPosition_.xyz - scene_.pointLights_[i].position_);

        float affect = 1.0f - 1.0f / scene_.pointLights_[i].range_ * distance;

        affect = max(0.0f, affect);

        affect = pow(affect, 3.0f);

        diffuse *= affect;
        specular *= affect;

        diffPoint += diffuse;
        specPoint += specular;
    }

	// --- �ŏI�I�ȃ��C�g�̐F ---
    float3 light = diffDirection + specDirection + diffPoint + specPoint + scene_.ambientLight_.color_;

	// --- �J���[�}�b�v�̎擾 ---
    float4 color = textureMaps[0].Sample(samplerStates[ANISOTROPIC], pin.texcoord_);

	// --- �t�K���}�␳ ---
    color.rgb = pow(color.rgb, scene_.invGamma_);

	// --- ���C�g�̐F����Z ---
    color.xyz *= light;

    float4 emissive = textureMaps[2].Sample(samplerStates[LINEAR], pin.texcoord_);
    emissive *= 3.0;

    color.xyz += emissive.xyz;

    return color * pin.color_;
}