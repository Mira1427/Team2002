#include "./Constant.hlsli"

// --- ���͗p�\���� ---
struct VSInput
{
	float4 position_ : POSITION;
};

// --- �o�͗p�\���� ---
struct VSOutput
{
	float4 position_	: SV_POSITION;
	float4 color_		: COLOR;
};


// --- �I�u�W�F�N�g�萔 ---
struct ObjectConstants
{
	row_major float4x4 world_;
	float4 color_;
};

// --- �I�u�W�F�N�g�萔2D ---
struct ObjectConstants2D
{
	float2	position_;
	float2	size_;
	float4	color_;
	float2	center_;
	float	rotation_;
};