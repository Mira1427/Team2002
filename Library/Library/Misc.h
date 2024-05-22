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



// --- エラーメッセージの取得 ---
inline LPWSTR hrTrace(HRESULT hr)
{
	LPWSTR msg = 0;

	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM |		// エラーメッセージの取得
		FORMAT_MESSAGE_IGNORE_INSERTS |		// 挿入文字列を無視
		FORMAT_MESSAGE_ALLOCATE_BUFFER,		// メッセージを格納するバッファを確保
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg),
		0,
		NULL
	);

	return msg;
}
