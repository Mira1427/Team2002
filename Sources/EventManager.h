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
	TO_OVER_SCENE,
};


enum class ButtonState
{
	NONE,
	TITLE,
	GAME,
	PAUSE,
	OVER,
	CLEAR
};


enum class TitleEvent
{
	START,
	END,
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


	void AddWaveCutIn();


	std::list<EventMessage> messages_;

	GameObject* stages_[static_cast<size_t>(StageDirection::MAX)];	// �X��4�������ꂽ����p
	bool paused_ = false;


	struct Button
	{
		ButtonState state_;
		int eventIndex_;
	}button_;
};

