#include "Constant.hlsli"

#include "PostEffect.hlsli"


Texture2D sceneTexture : register(t0);


PSBlurInput main( uint vertexID : SV_VertexID )
{
    PSBlurInput vout;
	

    const float2 positions[4] =
    {
        { -1, +1 },
        { +1, +1 },
        { -1, -1 },
        { +1, -1 }
    };

    const float2 texCoords[4] =
    {
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 }
    };
    
    // --- ���_���W������ ---
    vout.position_ = float4(positions[vertexID], 0.0, 1.0);

    
    // --- �e�N�X�`���T�C�Y�̎擾 ---
    float2 texSize;
    float level;
    sceneTexture.GetDimensions(0, texSize.x, texSize.y, level);
    
    // --- ��̃e�N�Z����UV���L�^ ---
    float2 tex = texCoords[vertexID];
    
    // --- ��e�N�Z������U���W��+1�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex0_.xy = float2(1.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+3�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex1_.xy = float2(3.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+5�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex2_.xy = float2(5.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+7�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex3_.xy = float2(7.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+9�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex4_.xy = float2(9.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+11�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex5_.xy = float2(11.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+13�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex6_.xy = float2(13.0 / texSize.x, 0.0);
    
    // --- ��e�N�Z������U���W��+15�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z���� ---
    vout.tex7_.xy = float2(15.0 / texSize.x, 0.0);
    
    
    // --- �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z���� ---
    vout.tex0_.zw = vout.tex0_.xy * -1.0;
    vout.tex1_.zw = vout.tex1_.xy * -1.0;
    vout.tex2_.zw = vout.tex2_.xy * -1.0;
    vout.tex3_.zw = vout.tex3_.xy * -1.0;
    vout.tex4_.zw = vout.tex4_.xy * -1.0;
    vout.tex5_.zw = vout.tex5_.xy * -1.0;
    vout.tex6_.zw = vout.tex6_.xy * -1.0;
    vout.tex7_.zw = vout.tex7_.xy * -1.0;
    
    
    // --- �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��āA���ۂɃT���v�����O����UV���W���v�Z���� ---
    vout.tex0_ += float4(tex, tex);
    vout.tex1_ += float4(tex, tex);
    vout.tex2_ += float4(tex, tex);
    vout.tex3_ += float4(tex, tex);
    vout.tex4_ += float4(tex, tex);
    vout.tex5_ += float4(tex, tex);
    vout.tex6_ += float4(tex, tex);
    vout.tex7_ += float4(tex, tex);
	
	return vout;
}