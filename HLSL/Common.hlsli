// --- �X�N���[�����W�n����NDC�ɕϊ� ---
float3 screenToNDC(const float2 position, const float2 screenSize)
{
	float x = 2.0f * position.x / screenSize.x - 1.0f;
	float y = 1.0f - 2.0f * position.y / screenSize.y;
	float z = 0.0f;

	return float3(x, y, z);
}


// --- �x���@����ʓx�@�ւ̕ϊ� ---
float toRadian(const float degree)
{
	float theta = 3.141592653589 / 180.0f * degree;
	return theta;
}


// --- �^�������̐��� ---
float rand(float n)
{
	return frac(sin(n) * 43758.5453123);
}


// --- Lambert�g�U���ˌ��̌v�Z ---
float3 calcLambertDiffuse(
	float3 lightDirection,
	float3 lightColor,
	float3 normal)
{
	// --- �s�N�Z���̖@���ƃ��C�g�̕����̓��ς̌v�Z ---
	float t = dot(normal, lightDirection) * -1.0f;

	// --- ���ς̒l��0�ȏ�ɂ��� ---
	t = max(0.0f, t);

	// --- �g�U���ˌ��̌v�Z ---
	return lightColor * t;
}


// --- Phong���ʔ��ˌ��̌v�Z ---
float3 calcPhongSpecular(
	float3 lightDirection,
	float3 lightColor,
	float3 eyePos,
	float3 worldPos,
	float3 normal)
{
	// --- ���˃x�N�g�������߂� ---
	float3 refVec = reflect(lightDirection, normal);

	// --- �������������T�[�t�F�X���王�_�ɐL�т�x�N�g�������߂� ---
	float3 toEye = eyePos - worldPos;
	toEye = normalize(toEye);

	// --- ���ʔ��˂̋��������߂� ---
	float t = dot(refVec, toEye);

	// --- ���ʔ��˂̋�����0�ȏ�̒l�ɂ��� ---
	t = max(0.0f, t);

	// --- ���ʔ��˂̋������i�� ---
	t = pow(t, 5.0f);

	// --- ���ʔ��ˌ������߂� ---
	return lightColor * t;
}


// --- �@���}�b�v����@�����v�Z ---
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


// --- �t���l�����˂��l�������g�U���˂̌v�Z ---
float CalcDiffuseFromFresnel(
	float3 N,	// �@��
	float3 L,	// �����Ɍ������x�N�g��
	float3 V	// �����Ɍ������x�N�g��
)
{
    // --- �����Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂� ---
    float3 H = normalize(L + V);

    // --- �e����0.5�ŌŒ� ---
    float roughness = 0.5;

    float energyBias = lerp(0.0, 0.5, roughness);
    float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);

    // --- �����Ɍ������x�N�g���ƃn�[�t�x�N�g�����ǂꂾ�����Ă��邩����ςŋ��߂� ---
    float dotLH = saturate(dot(L, H));

    // --- �����Ɍ������x�N�g���ƃn�[�t�x�N�g���A�������s�ɓ��˂����Ƃ��̊g�U���˗ʂ����߂� ---
    float fd90 = energyBias + 2.0 * dotLH * dotLH * roughness;

    // --- �@���ƌ����Ɍ������x�N�g��w�𗘗p���Ċg�U���˗������߂� ---
    float dotNL = saturate(dot(N, L));
    float FL = (1 + (fd90 - 1) * pow(1 - dotNL, 5));

    // --- �@���Ǝ��_�Ɍ������x�N�g���𗘗p���Ċg�U���˗������߂� ---
    float dotNV = saturate(dot(N, V));
    float FV = (1 + (fd90 - 1) * pow(1 - dotNV, 5));

    // --- �g�U���˗����m����Z���čŏI�I�Ȋg�U���˗������߂� ---
    return (FL * FV * energyFactor);
}


// --- �x�b�N�}�����z���v�Z���� ---
float CalcBeckmann(float m, float t)
{
    float t2 = t * t;
    float t4 = t * t * t * t;
    float m2 = m * m;
    float D = 1.0 / (4.0 * m2 * t4);
    D *= exp((-1.0 / m2) * (1.0 - t2) / t2);
	
    return D;
}


// --- �t���l���̌v�Z Schlick�ߎ����g�p ---
float CalcSpcFresnel(float f0, float u)
{
    return f0 + (1 - f0) * pow(1 - u, 5);
}


// --- Cook-Torrance���f���̋��ʔ��˂̌v�Z ---
float CalcCookTorranceSpecular(
	float3 L,
	float3 V,
	float3 N,
	float metaric
)
{
    float microfacet = 0.76;
	
	// --- �����x�𐂒����ˎ��̃t���l�����˗��Ƃ��Ĉ���
	//		�����x�������قǃt���l�����˂͑傫���Ȃ� ---
    float f0 = metaric;
	
	// --- ���C�g�Ɍ������x�N�g���Ǝ����Ɍ������x�N�g���̃n�[�t�x�N�g�������߂� ---
    float3 H = normalize(L + V);
	
	// --- �e��x�N�g�����ǂꂭ�炢���Ă��邩����ςŋ��߂� ---
    float dotNH = saturate(dot(N, H));
    float dotVH = saturate(dot(V, H));
    float dotNL = saturate(dot(N, L));
    float dotNV = saturate(dot(N, V));
	
	// --- D�����x�b�N�}�����z��p���Čv�Z���� ---
    float D = CalcBeckmann(microfacet, dotNH);
	
	// --- F����Schlick�ߎ���p���Čv�Z���� ---
    float F = CalcSpcFresnel(f0, dotVH);
	
	// --- G�������߂� ---
    float G = min(1.0, min(2 * dotNH * dotNV / dotVH, 2 * dotNH * dotNL / dotVH));
	
	// --- m�������߂� ---
    float m = 3.14159265358979 * dotNV * dotNH;
	
	// --- Cook-Torrance���f���̋��ʔ��˂����߂� ---
    return max(F * D * G / m, 0.0);
}