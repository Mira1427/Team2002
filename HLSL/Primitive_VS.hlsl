#include "Primitive.hlsli"

VSOutput main(VSInput vin) {

	VSOutput vout;
	vout.position_	= vin.position_;
	vout.color_		= vin.color_;

	return vout;
}