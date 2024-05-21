#include "./Constant.hlsli"

struct VSInput
{

    float4      position_ : POSITION;
    float4      normal_   : NORMAL;
    float4      tangent_  : TANGENT;
    float2      texcoord_ : TEXCOORD;
    float4x4    world_    : InstanceWorld;
    float4      color_    : InstanceColor;
};


struct VSOutput
{

    float4 position_        : SV_POSITION;
    float4 worldPosition_   : POSITION;
    float4 worldNormal_     : NORMAL;
    float4 worldTangent_    : TANGENT;
    float2 texcoord_        : TEXCOORD;
    float4 color_           : COLOR;
};