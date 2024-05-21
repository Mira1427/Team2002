#include "Constant.hlsli"
#include "Common.hlsli"

struct VSInput
{
	float3 position_			: POSITION;
	float2 instancePosition_	: INSTANCE_POSITION;
	float2 instanceSize_		: INSTANCE_SIZE;
	float4 instanceColor_		: INSTANCE_COLOR;
	float2 instanceCenter_		: INSTANCE_CENTER;
	float instanceAngle_		: INSTANCE_ANGLE;
};


struct VSOutput
{
	float4 position_	: SV_POSITION;
	float4 color_		: COLOR;
};