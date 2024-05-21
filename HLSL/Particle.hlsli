#include "Constant.hlsli"


// --- �p�[�e�B�N���萔 ---
struct ParticleConstant
{
    float3 emitterPosition_;
    float size_;
    float time_;
    float deltaTime_;
};


// --- �p�[�e�B�N���萔�o�b�t�@ ---
cbuffer CBParticle : register(OBJECT)
{
    ParticleConstant particle_;
}


// --- ���_�V�F�[�_�[�̏o�͍\���� ---
struct VSOutput
{
	uint vertexID_ : VERTEXID;
};


// --- �W�I���g���V�F�[�_�[�̏o�͍\���� ---
struct GSOutput
{
	float4 position_ : SV_POSITION;
	float4 color_ : COLOR;
	float2 texcoord_ : TEXCOORD;
};


// --- �p�[�e�B�N���̍\���� ---
struct Particle
{
	float4	color_;
	float3	position_;
	float	age_;
	float3	velocity_;
	int		state_;
};

#define NUMTHREADS_X 16