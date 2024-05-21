#include "InstancedMesh.hlsli"


VSOutput main(VSInput vin)
{
    vin.normal_.w = 0;
    float sigma = vin.tangent_.w;
    vin.tangent_.w = 0;

    VSOutput vout;
    //vout.position_ = mul(vin.position_, mul(vin.world_, scene_.viewProjection_));
    vout.position_ = mul(vin.world_, vin.position_);
    vout.position_ = mul(vout.position_, scene_.viewProjection_);
    vout.worldPosition_ = mul(vin.world_, vin.position_);
    vout.worldNormal_ = normalize(mul(vin.world_, vin.normal_));
    vout.worldTangent_ = normalize(mul(vin.world_, vin.tangent_));
    vout.worldTangent_.a = sigma;

    vout.texcoord_ = vin.texcoord_;

    vout.color_ = vin.color_;

    return vout;
}