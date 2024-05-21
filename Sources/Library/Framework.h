#pragma once

#include <memory>

#include "../Graphics/Primitive.h"


class Framework
{
public:
	void Run();
	bool Initialize();				// 初期化
	void Finalize();				// 終了処理
	void Update(float elapsedTime);	// 更新処理
	void Render();					// 描画処理
};

