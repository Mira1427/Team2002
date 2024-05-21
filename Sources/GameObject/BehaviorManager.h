#pragma once

#include <memory>
#include <string>
#include <unordered_map>


class Behavior;

class BehaviorManager
{
private:
	BehaviorManager();
	~BehaviorManager();

public:
	static BehaviorManager& Instance()
	{
		static BehaviorManager instance;
		return instance;
	}

	Behavior* GetBehavior(const char* key);

private:
	std::unordered_map<std::string, std::shared_ptr<Behavior>> behaviors_;
};

