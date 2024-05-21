#include "Constant.hlsli"

struct VSInput
{
	float4 position		: POSITION;
	float4 normal		: NORMAL;
	float4 tangent		: TANGENT;
	float2 texcoord		: TEXCOORD;
	float4 boneWeights	: WEIGHTS;
	uint4  boneIndices	: BONES;
};


struct VSOutput
{
	float4 position			: SV_POSITION;
	float4 worldPosition	: POSITION;
	float4 worldNormal		: NORMAL;
	float4 worldTangent		: TANGENT;
	float2 texcoord			: TEXCOORD;
	float4 color			: COLOR;
};


static const int MAXBONES = 512;


// --- �I�u�W�F�N�g�萔 ---
struct ObjectConstant
{
	row_major float4x4 world_;
	float4 color_;
	row_major float4x4 boneTransforms_[MAXBONES];
};


// --- �I�u�W�F�N�g�萔�o�b�t�@ ---
cbuffer CBObject : register(OBJECT)
{
	ObjectConstant object_;
}