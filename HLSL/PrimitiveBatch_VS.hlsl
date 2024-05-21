#include "PrimitiveBatch.hlsli"


VSOutput main(VSInput vin)
{
	VSOutput vout;

	float theta = toRadian(vin.instanceAngle_);
	float cosValue = cos(theta);
	float sinValue = sin(theta);

	float mx = vin.instanceSize_.x / vin.instanceSize_.x * vin.instanceCenter_.x;
	float my = vin.instanceSize_.y / vin.instanceSize_.y * vin.instanceCenter_.y;

	float2 position = vin.position_ * vin.instanceSize_;

	position.x -= mx;
	position.y -= my;

	float rx = position.x;
	float ry = position.y;
	position.x = rx * cosValue - ry * sinValue;
	position.y = rx * sinValue + ry * cosValue;

	position.x += mx;
	position.y += my;

	position.x += vin.instancePosition_.x - vin.instanceCenter_.x;
	position.y += vin.instancePosition_.y - vin.instanceCenter_.y;

	vout.position_ = float4(screenToNDC(position, scene_.windowSize_), 1.0f);
	vout.color_ = vin.instanceColor_;


	return vout;
}