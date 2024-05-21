// ===== 定数 ===============================================================================================================================================================================

// --- ブレンドステート用 ---
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

// --- テクスチャのラベル ---
#define DIFFUSE_MAP  0
#define NORMAL_MAP	 1
#define EMISSIVE_MAP 2
#define SHADOW_MAP   8


// --- バッファのラベル ---
#define OBJECT	b0
#define SCENE	b1
#define BLUR	b2

// --- モデルのマテリアル ---
#define ALBEDO_MAP 0
#define NORMAL_MAP 1
#define EMISSIVE_MAP 2
#define ROUGHNESS_MAP 3
#define METARIC_MAP 4

#define PI 3.14159265358979


// ===== 定数バッファ用構造体 ========================================================================================================================================================================================================

// --- 環境光定数 ---
struct AmbientLight
{
	float3 color_;
};


// --- ディレクションライト定数 ---
struct DirectionLight
{
    row_major float4x4 viewProjection_;
	float3	direction_;
	float	intensity_;
	float3	color_;
    float	pad_;
};


// --- ポイントライト定数 ---
struct PointLight
{
	float3	position_;
	float	intensity_;
	float3	color_;
	float	range_;
};

#define POINT_LIGHT_MAX 32


// --- シーン定数 ---
struct SceneConstant
{
	row_major float4x4	viewProjection_;	// カメラ行列
	row_major float4x4  invViewProjection_;	// カメラの逆行列
	float4				cameraPosition_;	// カメラの位置
	float2				windowSize_;		// ウィンドウのサイズ
	float				gamma_;				// ガンマ
	float				invGamma_;			// 逆ガンマ
	
    AmbientLight		ambientLight_;		// アンビエントライト
    DirectionLight		directionLight_;	// ディレクションライト
    PointLight			pointLights_[POINT_LIGHT_MAX];	// ポイントライト
};


// --- ブラー定数 ---
struct BlurConstant
{
    float4 weights[2];
};



// ===== 定数バッファ ======================================================================================================================================================

// --- シーン定数 ---
cbuffer CBScene : register(SCENE)
{
	SceneConstant scene_;
}


// --- ブラー定数 ---
cbuffer CBBlur : register(BLUR)
{
    BlurConstant blur_;
}