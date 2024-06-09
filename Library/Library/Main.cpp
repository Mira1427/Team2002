#include <Windows.h>

#include "Framework.h"
#include "Application.h"
#include "Library.h"


int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_  HINSTANCE prevInstance, _In_ LPSTR cmd_line, _In_ int cmd_show)
{
	RootsLib::Initialize(instance, L"Duality", 1920L, 1080L, false);

	Framework framework;

	if (framework.Initialize())
	{
		framework.Run();
	}

	framework.Finalize();
}
