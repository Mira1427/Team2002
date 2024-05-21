// --- スクリーン座標系からNDCに変換 ---
float3 screenToNDC(const float2 position, const float2 screenSize)
{
	float x = 2.0f * position.x / screenSize.x - 1.0f;
	float y = 1.0f - 2.0f * position.y / screenSize.y;
	float z = 0.0f;

	return float3(x, y, z);
}


// --- 度数法から弧度法への変換 ---
float toRadian(const float degree)
{
	float theta = 3.141592653589 / 180.0f * degree;
	return theta;
}


// --- 疑似乱数の生成 ---
float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}


// --- Lambert拡散反射光の計算 ---
float3 calcLambertDiffuse(
	float3 lightDirection,
	float3 lightColor,
	float3 normal)
{
	// --- ピクセルの法線とライトの方向の内積の計算 ---
	float t = dot(normal, lightDirection) * -1.0f;

	// --- 内積の値を0以上にする ---
	t = max(0.0f, t);

	// --- 拡散反射光の計算 ---
	return lightColor * t;
}


// --- Phong鏡面反射光の計算 ---
float3 calcPhongSpecular(
	float3 lightDirection,
	float3 lightColor,
	float3 eyePos,
	float3 worldPos,
	float3 normal)
{
	// --- 反射ベクトルを求める ---
	float3 refVec = reflect(lightDirection, normal);

	// --- 光が当たったサーフェスから視点に伸びるベクトルを求める ---
	float3 toEye = eyePos - worldPos;
	toEye = normalize(toEye);

	// --- 鏡面反射の強さを求める ---
	float t = dot(refVec, toEye);

	// --- 鏡面反射の強さを0以上の値にする ---
	t = max(0.0f, t);

	// --- 鏡面反射の強さを絞る ---
	t = pow(t, 5.0f);

	// --- 鏡面反射光を求める ---
	return lightColor * t;
}


// --- 法線マップから法線を計算 ---
float3 CalcNormalFromMap(
	float4 worldNormal,
	float4 worldTangent,
	float4 normalMap
)
{
    float3 N = normalize(worldNormal.xyz);
    float3 T = normalize(worldTangent.xyz);
    float sigma = worldTangent.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);
	
    float4 normal = (normalMap * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));
	
    return N.xyz;
}


// --- フレネル反射を考慮した拡散反射の計算 ---
float CalcDiffuseFromFresnel(
	float3 N,	// 法線
	float3 L,	// 光源に向かうベクトル
	float3 V	// 視線に向かうベクトル
)
{
    // --- 光源に向かうベクトルと視線に向かうベクトルのハーフベクトルを求める ---
    float3 H = normalize(L + V);

    // --- 粗さは0.5で固定 ---
    float roughness = 0.5;

    float energyBias = lerp(0.0, 0.5, roughness);
    float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

    // --- 光源に向かうベクトルとハーフベクトルがどれだけ似ているかを内積で求める ---
    float dotLH = saturate(dot(L, H));

    // --- 光源に向かうベクトルとハーフベクトル、光が平行に入射したときの拡散反射量を求める ---
    float fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

    // --- 法線と光源に向かうベクトルwを利用して拡散反射率を求める ---
    float dotNL = saturate(dot(N, L));
    float FL = (1 + (fd90 - 1) * pow(1 - dotNL, 5));

    // --- 法線と視点に向かうベクトルを利用して拡散反射率を求める ---
    float dotNV = saturate(dot(N, V));
    float FV = (1 + (fd90 - 1) * pow(1 - dotNV, 5));

    // --- 拡散反射率同士を乗算して最終的な拡散反射率を求める ---
    return (FL * FV * energyFactor);
}


// --- ベックマン分布を計算する ---
float CalcBeckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0 / (4.0 * m2 * t4);
    D *= exp((-1.0 / m2) * (1.0 - t2) / t2);
	
    return D;
}


// --- フレネルの計算 Schlick近似を使用 ---
float CalcSpcFresnel(float f0, float u)
{
    return f0 + (1 - f0) * pow(1 - u, 5);
}


// --- Cook-Torranceモデルの鏡面反射の計算 ---
float CalcCookTorranceSpecular(
	float3 L,
	float3 V,
	float3 N,
	float metaric
)
{
    float microfacet = 0.76;
	
	// --- 金属度を垂直入射時のフレネル反射率として扱う
	//		金属度が高いほどフレネル反射は大きくなる ---
    float f0 = metaric;
	
	// --- ライトに向かうベクトルと視線に向かうベクトルのハーフベクトルを求める ---
    float3 H = normalize(L + V);
	
	// --- 各種ベクトルがどれくらい似ているかを内積で求める ---
    float dotNH = saturate(dot(N, H));
    float dotVH = saturate(dot(V, H));
    float dotNL = saturate(dot(N, L));
    float dotNV = saturate(dot(N, V));
	
	// --- D項をベックマン分布を用いて計算する ---
    float D = CalcBeckmann(microfacet, dotNH);
	
	// --- F項をSchlick近似を用いて計算する ---
    float F = CalcSpcFresnel(f0, dotVH);
	
	// --- G項を求める ---
    float G = min(1.0, min(2 * dotNH * dotNV / dotVH, 2 * dotNH * dotNL / dotVH));
	
	// --- m項を求める ---
    float m = 3.14159265358979 * dotNV * dotNH;
	
	// --- Cook-Torranceモデルの鏡面反射を求める ---
    return max(F * D * G / m, 0.0);
}