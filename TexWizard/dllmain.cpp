#include "stdafx.h"
#include "TexWizard.h"

// DllMain function
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Initialize the D3D device
            if(!InitD3DDevice())
            {
                return FALSE;
            }
            
            // // Get the game folder path
            // std::string gameFolderPath = GetGameFolderPath();
            //
            // // Load texture mappings and bind textures asynchronously
            // std::string texturePacksPath = gameFolderPath + "\\NextGenGraphics\\TexturePacks\\";
            // LoadTextureMappings(texturePacksPath.c_str());
            // LoadAndBindTexturesAsync(texturePacksPath);
            //
            // // Hook the original LoadTexture function
            // originalLoadTexture = (OriginalLoadTextureFunc)GetProcAddress(
            //     GetModuleHandle("d3d9.dll"), "OriginalLoadTextureFunctionName");
            //
            // if (originalLoadTexture)
            // {
            //     DetourTransactionBegin();
            //     DetourUpdateThread(GetCurrentThread());
            //     DetourAttach(&(PVOID&)originalLoadTexture, HookedLoadTexture);
            //     DetourTransactionCommit();
            // }
            break;
        }

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
}
