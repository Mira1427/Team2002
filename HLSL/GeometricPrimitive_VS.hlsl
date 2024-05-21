#include "GeometricPrimitive.hlsli"

VSOutput main( VSInput vin )
{
    VSOutput vout;
	
    vout.position_  = mul(float4(vin.position_, 1.0), mul(vin.instanceWorld_, scene_.viewProjection_));
    vout.color_     = vin.instanceColor_;
	
	return vout;
}