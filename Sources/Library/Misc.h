#pragma once

#include <windows.h>
#include <crtdbg.h>

#if defined( DEBUG ) || defined( _DEBUG )
#define _ASSERT_EXPR_A(expr, msg) \
	(void)((!!(expr)) || \
	(1 != _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, NULL, "%s", msg)) || \
	(_CrtDbgBreak(), 0))
#else
#define  _ASSERT_EXPR_A(expr, expr_str) ((void)0)
#endif



// --- �G���[���b�Z�[�W�̎擾 ---
inline LPWSTR hrTrace(HRESULT hr)
{
	LPWSTR msg = 0;

	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM |		// �G���[���b�Z�[�W�̎擾
		FORMAT_MESSAGE_IGNORE_INSERTS |		// �}��������𖳎�
		FORMAT_MESSAGE_ALLOCATE_BUFFER,		// ���b�Z�[�W���i�[����o�b�t�@���m��
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg),
		0,
		NULL
	);

	return msg;
}
