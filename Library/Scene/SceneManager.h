#pragma once

#include <memory>


struct ID3D11DeviceContext;


// --- シーンの基底クラス ---
class Scene
{
public:
	virtual void Initialize() { state_ = 0; ready_ = false; }
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(ID3D11DeviceContext* dc) = 0;

	bool IsReady() const { return ready_; }
	void SetReady() { ready_ = true; }

protected:
	int state_;
	bool ready_ = false;
};


// --- シーン管理クラス ---
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}


	// --- 現在のシーンの取得 ---
	Scene* GetCurrentScene() { return currentScene_.get(); }

	// --- 次のシーンの取得 ---
	Scene* GetNextScene() { return nextScene_.get(); }

	// --- 次のシーンの設定 ---
	void SetNextScene(std::shared_ptr<Scene> scene) { nextScene_ = scene; }

	// --- 現在のシーンを直接変更 ---
	void SetCurrentScene(std::shared_ptr<Scene> scene) { currentScene_ = scene; }

	// --- 初期化 ---
	void Initialize(std::shared_ptr<Scene> scene) { SetNextScene(scene); }

	// --- 更新処理 ---
	void Update(float elapsedTime);

	// --- 描画処理 ---
	void Draw(ID3D11DeviceContext* dc);

	// --- シーンの変更処理 ---
	void ChangeScene();


private:
	std::shared_ptr<Scene> currentScene_ = nullptr;	// 現在のシーン
	std::shared_ptr<Scene> nextScene_ = nullptr;	// 次のシーン
};

