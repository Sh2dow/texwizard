#include "stdafx.h"
#include "stdio.h"
#include <string>
#include <windows.h>
#include "external/inireader/IniReader.h"
#include "external/Detours/src/detours.h"
#include "TexWizard.h"

void InitD3DDevice()
{
    // Obtain the Direct3D device from the game
    // This might be specific to the game's internal implementation
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        InitD3DDevice();
        LoadTextureMappings("NextGenGraphics/TexturePacks/VanillaPlus/TexturePackInfo.json");

        originalLoadTexture = (OriginalLoadTextureFunc)GetProcAddress(
            GetModuleHandle("d3d9.dll"), "OriginalLoadTextureFunctionName");
        if (originalLoadTexture)
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)originalLoadTexture, HookedLoadTexture);
            DetourTransactionCommit();
        }
        break;
    case DLL_PROCESS_DETACH:
        if (originalLoadTexture)
        {
            DetourTransactionBegin();
            DetourUpdateThread(GetCurrentThread());
            DetourDetach(&(PVOID&)originalLoadTexture, HookedLoadTexture);
            DetourTransactionCommit();
        }
        break;
    }
    return TRUE;

    // 	if (reason == DLL_PROCESS_ATTACH)
    // 	{
    // 		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
    // 		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
    // 		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);
    //
    // #ifdef GAME_PS
    //
    // 		if (strstr((const char*)(base + (0xA49742 - base)), "ProStreet08Release.exe"))
    // 		{
    // 			Init();
    // 		}
    // #else
    // 		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == EntryPoint) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
    // 		{
    // 			Init();
    // 		}
    // #endif
    // 		else
    // 		{
    // 			MessageBoxA(NULL, WrongEntryPointErrorString, "TexWizard", MB_ICONERROR);
    // 			return FALSE;
    // 		}
    // 	}
    // 	return TRUE;
}
