// dllmain.cpp : Defines the entry point for the DLL application.
//#include "stdafx.h"
//#include "Function.h"
#include "detours.h"
#include "windows.h"

using namespace std;

// JD96 perfer Typedefs. He think Typedefs are sexy, use them
// typedef void(__thiscall* tPrint)(DWORD dwThis, int num, char *txt);
void(__thiscall* orignalPrintChat) (void*, int, const char*);

DWORD Printaddy;

//fastcall workarround because you cant declare a function as thiscall without a class
//behaves same as thiscall but has ECX and EDX as first params, not only ECX. And here ecx holds the address of object! Remember there is EDX. Otherwises num will be replaced by EDX. 
void __fastcall DetourPrintChat(void* pTHIS, DWORD EDX, int num, char* txt)
{
	orignalPrintChat(pTHIS, num, "Detour success");
}

// Here is the example that __thiscall is precreate. So THIS is not neccessary because in the MyFirstCpp.exe, we don't need the property of object. If the
// program need the property of object but you don't provide. Your dll will crash.
void PrintChat(int num, char* txt, DWORD Printaddy)
{
	__asm
	{
		push txt
		push 1
		call Printaddy
	}
}



void InitiateHook()
{
	Printaddy = FindPattern("MyFirstCpp.exe", "\xF3\xAB\x59\x89\x4D\xF8\x83\x7D\x08\x01\x75\x29", "xxxxxxxxxxxx");
	Printaddy = Printaddy - 0x1D;
	cout << "success" << endl;
	orignalPrintChat = (void(__thiscall*) (void*, int, char*)) DetourFunction((PBYTE)Printaddy, (PBYTE)DetourPrintChat);

}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//You should create a thread, some injectors are retarded :D
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitiateHook, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}