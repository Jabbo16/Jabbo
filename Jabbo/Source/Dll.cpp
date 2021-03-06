#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <BWAPI.h>
#include "Jabbo.hpp"

extern "C" __declspec(dllexport) void gameInit(Game* game) { BroodwarPtr = game; }
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

extern "C" __declspec(dllexport) AIModule* newAIModule()
{
	return new Jabbo::JabboModule();
}