#pragma once

#include <memory>
#include <string>
#include <unordered_map>


class Eraser;

class EraserManager
{
private:
	EraserManager();
	~EraserManager();

public:
	static EraserManager& Instance()
	{
		static EraserManager instance;
		return instance;
	}

	Eraser* GetEraser(const char* key);

private:
	std::unordered_map<std::string, std::shared_ptr<Eraser>> erasers_;
};

