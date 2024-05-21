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
    
    // --- 頂点座標を決定 ---
    vout.position_ = float4(positions[vertexID], 0.0, 1.0);

    
    // --- テクスチャサイズの取得 ---
    float2 texSize;
    float level;
    sceneTexture.GetDimensions(0, texSize.x, texSize.y, level);
    
    // --- 基準のテクセルのUVを記録 ---
    float2 tex = texCoords[vertexID];
    
    // --- 基準テクセルからU座標を+1テクセルずらすためのオフセットを計算する ---
    vout.tex0_.xy = float2(1.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+3テクセルずらすためのオフセットを計算する ---
    vout.tex1_.xy = float2(3.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+5テクセルずらすためのオフセットを計算する ---
    vout.tex2_.xy = float2(5.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+7テクセルずらすためのオフセットを計算する ---
    vout.tex3_.xy = float2(7.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+9テクセルずらすためのオフセットを計算する ---
    vout.tex4_.xy = float2(9.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+11テクセルずらすためのオフセットを計算する ---
    vout.tex5_.xy = float2(11.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+13テクセルずらすためのオフセットを計算する ---
    vout.tex6_.xy = float2(13.0 / texSize.x, 0.0);
    
    // --- 基準テクセルからU座標を+15テクセルずらすためのオフセットを計算する ---
    vout.tex7_.xy = float2(15.0 / texSize.x, 0.0);
    
    
    // --- オフセットに-1を掛けてマイナス方向のオフセットも計算する ---
    vout.tex0_.zw = vout.tex0_.xy * -1.0;
    vout.tex1_.zw = vout.tex1_.xy * -1.0;
    vout.tex2_.zw = vout.tex2_.xy * -1.0;
    vout.tex3_.zw = vout.tex3_.xy * -1.0;
    vout.tex4_.zw = vout.tex4_.xy * -1.0;
    vout.tex5_.zw = vout.tex5_.xy * -1.0;
    vout.tex6_.zw = vout.tex6_.xy * -1.0;
    vout.tex7_.zw = vout.tex7_.xy * -1.0;
    
    
    // --- オフセットに基準テクセルのUV座標を足して、実際にサンプリングするUV座標を計算する ---
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