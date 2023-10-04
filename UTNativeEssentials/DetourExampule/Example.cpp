#include <Windows.h>
#include "iostream"

#include "detours.h"

static int (WINAPI* NativeMessageBox)(
	HWND    hWnd,
	LPCTSTR lpText,
	LPCTSTR lpCaption,
	UINT    uType
	) = MessageBox;

int WINAPI MyMessageBox(
	HWND    hWnd,
	LPCTSTR lpText,
	LPCTSTR lpCaption,
	UINT    uType)
{
	return NativeMessageBox(hWnd, L"Detours Hooking!", lpCaption, uType);
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{

	if (DetourIsHelperProcess())
	{
		return TRUE;
	}

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		std::cout << "Inside dll injection routine";
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)NativeMessageBox, MyMessageBox);
		DetourTransactionCommit();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)NativeMessageBox, MyMessageBox);
		DetourTransactionCommit();
		break;
	}

	return TRUE;
}