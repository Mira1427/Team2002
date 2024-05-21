struct VSOutput
{
    float4 position_ : SV_POSITION;
    float4 color_ : COLOR;
    float2 texcoord_ : TEXCOORD;
};


struct PSBlurInput
{
    float4 position_ : SV_Position;
    float4 tex0_     : TEXCOORD0;
    float4 tex1_     : TEXCOORD1;
    float4 tex2_     : TEXCOORD2;
    float4 tex3_     : TEXCOORD3;
    float4 tex4_     : TEXCOORD4;
    float4 tex5_     : TEXCOORD5;
    float4 tex6_     : TEXCOORD6;
    float4 tex7_     : TEXCOORD7;
};