#include "PrimitiveBatch.hlsli"

float4 main(VSOutput pin) : SV_TARGET
{
	return pin.color_;
}