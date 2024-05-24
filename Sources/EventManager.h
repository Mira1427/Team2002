#pragma once


class GameObject;


enum class StageDirection
{
	LEFT,
	DOWN,
	RIGHT,
	UP,
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

	GameObject* stages_[static_cast<size_t>(StageDirection::MAX)];	// 街の4等分された判定用
};

