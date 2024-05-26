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

	void TranslateMessage(EventMessage msg) { messages_.emplace_back(msg); }

	std::list<EventMessage> messages_;

	GameObject* stages_[static_cast<size_t>(StageDirection::MAX)];	// ŠX‚Ì4“™•ª‚³‚ê‚½”»’è—p
	bool paused_ = false;
};

