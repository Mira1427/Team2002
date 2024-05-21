#include "./DebugRenderer.hlsli"
#include "./Common.hlsli"

// --- 2D�I�u�W�F�N�g�萔�o�b�t�@ ---
cbuffer CBObject2D : register(b0)
{
	ObjectConstants2D object_;
}


VSOutput main(in VSInput vin)
{
	// --- �o�͗p�ϐ� ---
	VSOutput vout;

	float theta = toRadian(object_.rotation_);
	float cosValue = cos(theta);
	float sinValue = sin(theta);

	float mx = object_.size_.x / object_.size_.x * object_.center_.x;
	float my = object_.size_.y / object_.size_.y * object_.center_.y;

	float2 position = vin.position_.xy * object_.size_;

	position.x -= mx;
	position.y -= my;

	float rx = position.x;
	float ry = position.y;
	position.x = rx * cosValue - ry * sinValue;
	position.y = rx * sinValue + ry * cosValue;

	position.x += mx;
	position.y += my;

	position.x += object_.position_.x - object_.center_.x;
	position.y += object_.position_.y - object_.center_.y;

	vout.position_ = float4(screenToNDC(position, scene_.windowSize_.xy), 1.0f);
	vout.color_ = object_.color_;

	return vout;
}