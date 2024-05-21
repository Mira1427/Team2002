#include "DebugRenderer.hlsli"

// --- �I�u�W�F�N�g�萔�o�b�t�@ ---
cbuffer CBObject : register(b0)
{
	ObjectConstants object_;
}


VSOutput main( VSInput vin )
{
	VSOutput vout;

	vout.position_ = mul(vin.position_, mul(object_.world_, scene_.viewProjection_));
	vout.color_ = object_.color_;

	return vout;
}