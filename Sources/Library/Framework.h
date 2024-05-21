#pragma once

#include <memory>

#include "../Graphics/Primitive.h"


class Framework
{
public:
	void Run();
	bool Initialize();				// ������
	void Finalize();				// �I������
	void Update(float elapsedTime);	// �X�V����
	void Render();					// �`�揈��
};

