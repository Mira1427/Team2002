#pragma once

#include <memory>


struct ID3D11DeviceContext;


// --- �V�[���̊��N���X ---
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


// --- �V�[���Ǘ��N���X ---
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


	// --- ���݂̃V�[���̎擾 ---
	Scene* GetCurrentScene() { return currentScene_.get(); }

	// --- ���̃V�[���̎擾 ---
	Scene* GetNextScene() { return nextScene_.get(); }

	// --- ���̃V�[���̐ݒ� ---
	void SetNextScene(std::shared_ptr<Scene> scene) { nextScene_ = scene; }

	// --- ���݂̃V�[���𒼐ڕύX ---
	void SetCurrentScene(std::shared_ptr<Scene> scene) { currentScene_ = scene; }

	// --- ������ ---
	void Initialize(std::shared_ptr<Scene> scene) { SetNextScene(scene); }

	// --- �X�V���� ---
	void Update(float elapsedTime);

	// --- �`�揈�� ---
	void Draw(ID3D11DeviceContext* dc);

	// --- �V�[���̕ύX���� ---
	void ChangeScene();


private:
	std::shared_ptr<Scene> currentScene_ = nullptr;	// ���݂̃V�[��
	std::shared_ptr<Scene> nextScene_ = nullptr;	// ���̃V�[��
};

