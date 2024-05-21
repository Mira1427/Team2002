// ===== �萔 ===============================================================================================================================================================================

// --- �u�����h�X�e�[�g�p ---
#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

// --- �e�N�X�`���̃��x�� ---
#define DIFFUSE_MAP  0
#define NORMAL_MAP	 1
#define EMISSIVE_MAP 2
#define SHADOW_MAP   8


// --- �o�b�t�@�̃��x�� ---
#define OBJECT	b0
#define SCENE	b1
#define BLUR	b2

// --- ���f���̃}�e���A�� ---
#define ALBEDO_MAP 0
#define NORMAL_MAP 1
#define EMISSIVE_MAP 2
#define ROUGHNESS_MAP 3
#define METARIC_MAP 4

#define PI 3.14159265358979


// ===== �萔�o�b�t�@�p�\���� ========================================================================================================================================================================================================

// --- �����萔 ---
struct AmbientLight
{
	float3 color_;
};


// --- �f�B���N�V�������C�g�萔 ---
struct DirectionLight
{
    row_major float4x4 viewProjection_;
	float3	direction_;
	float	intensity_;
	float3	color_;
    float	pad_;
};


// --- �|�C���g���C�g�萔 ---
struct PointLight
{
	float3	position_;
	float	intensity_;
	float3	color_;
	float	range_;
};

#define POINT_LIGHT_MAX 32


// --- �V�[���萔 ---
struct SceneConstant
{
	row_major float4x4	viewProjection_;	// �J�����s��
	row_major float4x4  invViewProjection_;	// �J�����̋t�s��
	float4				cameraPosition_;	// �J�����̈ʒu
	float2				windowSize_;		// �E�B���h�E�̃T�C�Y
	float				gamma_;				// �K���}
	float				invGamma_;			// �t�K���}
	
    AmbientLight		ambientLight_;		// �A���r�G���g���C�g
    DirectionLight		directionLight_;	// �f�B���N�V�������C�g
    PointLight			pointLights_[POINT_LIGHT_MAX];	// �|�C���g���C�g
};


// --- �u���[�萔 ---
struct BlurConstant
{
    float4 weights[2];
};



// ===== �萔�o�b�t�@ ======================================================================================================================================================

// --- �V�[���萔 ---
cbuffer CBScene : register(SCENE)
{
	SceneConstant scene_;
}


// --- �u���[�萔 ---
cbuffer CBBlur : register(BLUR)
{
    BlurConstant blur_;
}