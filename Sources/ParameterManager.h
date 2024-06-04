#pragma once

#include <memory>

#include "../../Library/Graphics/Effect.h"


class ParameterManager
{
private:
	ParameterManager() {};
	~ParameterManager() {};

public:
	static ParameterManager& Instance()
	{
		static ParameterManager instance;
		return instance;
	}

	void InitializeEffects();


	std::unique_ptr<Effect> laserEffect_;
	std::unique_ptr<Effect> smokeEffect_;
};

