#include "Particle.hlsli"

#include "Common.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

[numthreads(NUMTHREADS_X, 1, 1)]
void main( uint3 dtID : SV_DispatchThreadID )
{
	uint id = dtID.x;

	Particle p = particleBuffer[id];

    if (p.age_ > 10.0f)
    {
        //const float g = -0.5f;
        //p.velocity_.y += g * particle_.deltaTime_;
        p.position_ += p.velocity_ * particle_.deltaTime_;

        if (p.position_.y > 200.0)
        {      
            const float noiseScale = 1.0f;
            float f0 = rand(id * noiseScale);
            float f1 = rand(f0 * noiseScale);
            float f2 = rand(f1 * noiseScale);

            p.position_ = float3(f0 * 200.0 - 100.0f, f1 * 100.0, f2 * 200.0 - 100.0);
        }
    }

    p.age_ += particle_.deltaTime_;

	particleBuffer[id] = p;
}