#include "Constant.hlsli"

struct VSInput
{
    float3              position_      : Position;
    row_major float4x4  instanceWorld_ : InstanceWorld;
    float4               instanceColor_ : InstanceColor;
};


struct VSOutput
{
    float4 position_ : SV_Position;
    float4 color_    : Color;
};