#pragma once

#include <string>
#include <memory>
#include <vector>
#include <set>

#include <d3d11.h>

#include "../Math/Matrix.h"

#include "../Graphics/Graphics.h"
#include "../Graphics/MeshRenderer.h"
#include "../Graphics/InstancedMesh.h"
#include "../Graphics/TextureManager.h"



// ===== 前方宣言 ============================================================================================================================================
class GameObject;


// ===== コンポーネント =======================================================================================================================================
class Component
{
public:
	Component() :
		object_(nullptr)
	{}

	virtual void Draw(ID3D11DeviceContext* dc) {}
	virtual void UpdateDebugGui(float elapsedTime) {};
	void SetObject(GameObject* object) { object_ = object; }

protected:
	GameObject* object_;
};


// ===== トランスフォーム ======================================================================================================================================================
class Transform final : public Component
{
public:
	Transform() :
		world_(),
		position_(),
		scaling_(Vector3::Unit_),
		rotation_(),
		coordinateSystem_(1/*左手系 Y軸アップ*/)
	{}

	// --- 姿勢行列の作成 ---
	void MakeTransform();

	// --- デバッグGuiの更新 ---
	void UpdateDebugGui(float elapsedTime) override;

	Matrix world_;
	Vector3 position_;
	float moveSpeed_ = 0.01f;
	Vector3 scaling_;
	float scaleSpeed_ = 0.01f;
	Vector3 rotation_;
	float rotateSpeed_ = 1.0f;
	int coordinateSystem_;

	static const Matrix CoordinateSystems_[4];
};


class SpriteRendererComponent final : public Component
{
public:
	SpriteRendererComponent() :
		textureName_(),
		texture_(nullptr),
		texPos_(),
		texSize_(),
		center_(),
		color_(Vector4::White_),
		blendState_(1/*Alpha*/),
		rasterState_(0/*CullNone*/),
		testDepth_(false),
		writeDepth_(false),
		inWorld_(),
		isVisible_(true),
		useBillboard_()
	{}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;


	std::string textureName_;
	Texture* texture_;
	Vector2 texPos_;
	Vector2 texSize_;
	Vector2 center_;
	Vector4 color_;
	int		blendState_;
	int		rasterState_;
	bool	testDepth_;
	bool	writeDepth_;
	bool	inWorld_;
	bool	isVisible_;
	bool	useBillboard_;
};


struct AnimationFrameData
{
	float frame_;
	Vector2 texPos_;
	Vector2 texSize_;
};

struct AnimationData
{
	AnimationData() {}

	AnimationData(const std::string& name, Texture* texture, std::vector<AnimationFrameData>* frameData) :
		name_(name),
		texture_(texture),
		frameData_(frameData)
	{}

	std::string name_;
	Texture* texture_;
	std::vector<AnimationFrameData>* frameData_;
};

class AnimationComponent final : public Component
{
public:
	AnimationComponent() :
		animationClips_(nullptr),
		frame_(),
		animationIndex(),
		frameIndex(),
		timeScale_(1.0f),
		startFrame_(),
		endFrame_(),
		isLoop_(),
		isPlay_()
	{}

	void Update(float elapsedTime);

	void UpdateDebugGui(float elapsedTime) override;

	std::vector<AnimationData>* animationClips_;
	float frame_;
	int animationIndex;
	int frameIndex;
	float timeScale_;
	int startFrame_;
	int endFrame_;
	bool isLoop_;
	bool isPlay_;
};


// --- スケルタルメッシュのコンポーネント ---
class MeshRendererComponent final : public Component
{
public:
	MeshRendererComponent() :
		model_(nullptr),
		modelName_(u8"モデル"),
		color_(Vector4::White_),
		meshIndex(),
		boneIndex(),
		blendState_(1/*Alpha*/),
		rasterState_(1/*CullBack*/),
		testDepth_(true),
		writeDepth_(true),
		isVisible_(true)
	{}

	// --- 描画処理 ---
	void Draw(ID3D11DeviceContext* dc) override;

	// --- デバッグGuiの更新 ---
	void UpdateDebugGui(float elapsedTime) override;

	MeshRenderer* model_;
	std::string modelName_;
	Vector4 color_;
	int meshIndex;
	int boneIndex;
	int blendState_;
	int rasterState_;
	bool testDepth_;
	bool writeDepth_;
	bool isVisible_;
};


class AnimatorComponent final : public Component
{
public:
	AnimatorComponent() :
		keyFrame_(nullptr),
		animationIndex(),
		keyFrameIndex(),
		startFrame_(),
		endFrame_(),
		frame_(),
		timeScale_(1.0f),
		isLoop_(),
		isPlay_(),
		nodeIndex(),
		translation_(),
		scaling_(1.0f, 1.0f, 1.0f),
		rotation_()
	{}

	void Update(float elapsedTime);
	void UpdateNodeTransform();

	void UpdateDebugGui(float elapsedTime) override;


	Animation::KeyFrame* keyFrame_;
	int		animationIndex;
	int		keyFrameIndex;
	int		startFrame_;
	int		endFrame_;
	float	frame_;
	float	timeScale_;
	bool	isLoop_;
	bool	isPlay_;

	int		nodeIndex;
	Vector3 translation_;
	Vector3 scaling_;
	Vector3 rotation_;
};


// --- インスタンスメッシュのコンポーネント ---
class InstancedMeshComponent final : public Component
{
public:
	InstancedMeshComponent() :
		model_(nullptr),
		modelName_(u8"モデル"),
		color_(Vector4::White_),
		meshIndex(),
		blendState_(1/*Alpha*/),
		rasterState_(1/*CullBack*/),
		testDepth_(true),
		writeDepth_(true),
		isVisible_(true)
	{}

	// --- 描画処理 ---
	void Draw(ID3D11DeviceContext* dc) override;

	// --- デバッグGuiの更新 ---
	void UpdateDebugGui(float elapsedTime) override;

	InstancedMesh* model_;
	std::string		modelName_;
	Vector4			color_;
	int				meshIndex;
	int				blendState_;
	int				rasterState_;
	bool			testDepth_;
	bool			writeDepth_;
	bool			isVisible_;
};



class PrimitiveRendererComponent final : public Component
{
public:
	PrimitiveRendererComponent() :
		size_(),
		center_(),
		color_(Vector4::White_),
		blendState_(1/*Alpha*/),
		rasterState_(0/*CullNone*/),
		testDepth_(false),
		writeDepth_(false),
		isVisible_(true)
	{}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;

	Vector2 size_;
	Vector2 center_;
	Vector4 color_;
	int		blendState_;
	int		rasterState_;
	bool	testDepth_;
	bool	writeDepth_;
	bool	isVisible_;
};


class ColliderComponent : public Component
{
public:
	enum class CollisionType
	{
		BOX2D,
		CIRCLE,
		BOX,
		CAPSULE,
		SPHERE,
	};

	CollisionType collisionType_;
	bool isHit_ = false;
	bool isVisible_ = false;
};

class BoxCollider2D final : public ColliderComponent
{
public:
	BoxCollider2D() :
		offset_(),
		size_()
	{
		collisionType_ = CollisionType::BOX2D;
	}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;


	Vector2 size_;
	Vector2 offset_;
};

class CircleCollider final : public ColliderComponent
{
public:
	CircleCollider() :
		offset_(),
		radius_()
	{
		collisionType_ = CollisionType::CIRCLE;
	}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;


	float	radius_;
	Vector2 offset_;
};

class BoxCollider final : public ColliderComponent
{
public:
	BoxCollider() :
		size_(),
		offset_()
	{
		collisionType_ = CollisionType::BOX;
	}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;


	Vector3 size_;
	Vector3 offset_;
};

class CapsuleCollider final : public ColliderComponent
{
public:
	CapsuleCollider() :
		radius_(),
		height_(),
		offset_()
	{
		collisionType_ = CollisionType::CAPSULE;
	}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;


	float radius_;
	float height_;
	Vector3 offset_;
};

class SphereCollider final : public ColliderComponent
{
public:
	SphereCollider() :
		radius_(),
		offset_()
	{
		collisionType_ = CollisionType::SPHERE;
	}

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;


	float radius_;
	Vector3 offset_;
};


class PointLightComponent final : public Component
{
public:
	PointLightComponent() :
		color_(1.0f, 1.0f, 1.0f),
		range_(),
		intensity_(1.0f),
		isVisible_(),
		isOn_(true)
	{}

	void Update();

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;

	Vector3 color_;
	float range_;
	float intensity_;
	bool isVisible_;
	bool isOn_;
};


class CameraComponent final : public Component
{
public:
	CameraComponent() :
		viewProjection_(),
		invViewProjection_(),
		view_(),
		invView_(),
		projection_(),
		frontVec_(),
		upVec_(),
		rightVec_(),
		target_(),
		fov_(60.0f),
		nearZ_(0.1f),
		farZ_(1000.0f),
		range_(25.0f),
		isVisible_()
	{}

	void Update();

	void Draw(ID3D11DeviceContext* dc) override;

	void UpdateDebugGui(float elapsedTime) override;

	Matrix viewProjection_;
	Matrix invViewProjection_;
	Matrix view_;
	Matrix invView_;
	Matrix projection_;
	Vector3 frontVec_;
	Vector3 upVec_;
	Vector3 rightVec_;
	Vector3 target_;
	float fov_;
	float nearZ_;
	float farZ_;
	float range_;
	bool isVisible_;
};


// --- 剛体コンポーネント ---
class RigidBodyComponent final : public Component
{
public:
	RigidBodyComponent() :
		velocity_(),
		accel_(),
		gravity_(),
		force_(),
		friction_()
	{}

	void Draw(ID3D11DeviceContext* dc) override {};

	void UpdateDebugGui(float elapsedTime) override;

	Vector3 velocity_;
	Vector3 accel_;
	Vector3 gravity_;
	Vector3 force_;
	Vector3 friction_;
};


// ===== 行動クラス =======================================================================================================================================
class Behavior
{
public:
	// --- 毎フレーム呼び出される更新処理関数 ---
	virtual void Execute(GameObject* object, float elapsedTime) = 0;

	// --- オブジェクトが衝突した際に呼び出される関数 ---
	virtual void Hit(GameObject* src, GameObject* dst, float elapsedTime) = 0;
};


// ===== 消去クラス =======================================================================================================================================
class Eraser
{
public:
	// --- 更新処理が終わった後に呼び出される純粋仮想関数 ---
	virtual void Execute(GameObject* object) = 0;
};


// ===== ゲームオブジェクトクラス =============================================================================================================================

// --- 当たり判定のための定数 ---
enum class ObjectType
{
	NONE,
	BULLET,
	SPAWNER,
	ENEMY,
};

class GameObject
{
public:
	GameObject();

	// --- 更新処理 ---
	void Update(float elapsedTime);

	// --- 削除 ---
	void Destroy();

	// --- コンポーネントの追加 ---
	template<typename T> T* AddComponent()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		component->SetObject(this);
		components_.emplace_back(component);
		return component.get();
	}

	template<typename T> T* AddCollider()
	{
		std::shared_ptr<T> component = std::make_shared<T>();
		component->SetObject(this);
		components_.emplace_back(component);
		colliders_.emplace_back(component.get());
		return component.get();
	}

	// --- コンポーネントの取得 ---
	template<typename T> T* GetComponent()
	{
		for (auto& component : components_)
		{
			auto castedComponent = std::dynamic_pointer_cast<T>(component);

			if (castedComponent)
				return castedComponent.get();
		}

		return nullptr;
	}

	std::string	name_;
	int			state_;
	float		timer_;
	bool		isChoose_;
	ObjectType	type_;

	Behavior* behavior_ = nullptr;
	Eraser* eraser_ = nullptr;

	std::vector<std::shared_ptr<Component>> components_;
	std::vector<ColliderComponent*> colliders_;
	std::shared_ptr<Transform> transform_;

	GameObject* parent_;
	std::vector<GameObject*> child_;

};


// ===== ゲームオブジェクト管理クラス ========================================================================================================================
class GameObjectManager
{
private:

	GameObjectManager() {};
	~GameObjectManager() {};
	GameObjectManager(const GameObjectManager&) = delete;
	GameObjectManager& operator=(const GameObjectManager&) = delete;
	GameObjectManager(GameObjectManager&&) noexcept = delete;
	GameObjectManager& operator=(GameObjectManager&&) noexcept = delete;

public:
	// --- インスタンスの取得 ---
	static GameObjectManager& Instance()
	{
		static GameObjectManager instance;
		return instance;
	}


	void Update(float elapsedTime);
	void Draw(ID3D11DeviceContext* dc);
	void JudgeCollision(float elapsedTime);
	void Remove();
	void UpdateDebugGui(float elapsedTime);
	void ShowDebugList();

	// --- オブジェクトの追加 ---
	GameObject* Add(std::shared_ptr<GameObject> object, const Vector3& position = Vector3::Zero_, Behavior* behavior = nullptr);


	std::list<std::shared_ptr<GameObject>> objectList_;	// オブジェクトのリスト
	std::set<GameObject*> discardList_;					// 削除予定のオブジェクトのリスト


	bool isSort_ = false;
	bool castShadow_ = false;
	bool showCollision_ = false;
};