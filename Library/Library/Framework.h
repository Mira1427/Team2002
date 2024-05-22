#pragma once

#include <memory>

#include "../Graphics/Primitive.h"


class Framework
{
public:
	void Run();
	bool Initialize();				// ‰Šú‰»
	void Finalize();				// I—¹ˆ—
	void Update(float elapsedTime);	// XVˆ—
	void Render();					// •`‰æˆ—
};

