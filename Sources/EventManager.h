#pragma once

#include <list>

class GameObject;


enum class StageDirection
{
	LEFT,
	DOWN,
	RIGHT,
	UP,
	MAX
};


enum class EventMessage
{
	TO_TITLE_SCENE,
	TO_GAME_SCENE,
	TO_CLEAR_SCENE,
	TO_RESULT_SCENE,
};


enum class ButtonState
{
	NONE,
	TITLE,
	GAME,
	PAUSE,
	OVER,
	CLEAR,
	SCENE_BEGIN,
};


enum class TitleEvent
{
	START,
	MAX
};


enum class TutorialSelectEvent
{
	ON,
	OFF,
	MAX
};


enum class PauseEvent
{
	CONTINUE,
	END,
	MAX
};


class EventManager
{
private:
	EventManager() {}
	~EventManager() {}

public:
	static EventManager& Instance()
	{
		static EventManager instance;
		return instance;
	}

	void Initialize();
	void Update(float elapsedTime);
	void UpdateButton();
	void UpdateDebugGui();

	void TranslateMessage(EventMessage msg) { messages_.emplace_back(msg); }


	void UpdateTitleEvent();
	void UpdateGameEvent();
	void UpdatePauseEvent();
	void UpdateSceneBeginEvent();


	void AddWaveCutIn();
	void AddColorItem();
	void AddGaugeItem();

	void InitializeObjects();


	std::list<EventMessage> messages_;

	GameObject* stages_[static_cast<size_t>(StageDirection::MAX)];	// ŠX‚Ì4“™•ª‚³‚ê‚½”»’è—p
	GameObject* enemySpawner_;
	GameObject* controller_;
	bool paused_ = false;


	struct Button
	{
		ButtonState state_;
		int eventIndex_;
		int subEventIndex_;
	}button_;
};

