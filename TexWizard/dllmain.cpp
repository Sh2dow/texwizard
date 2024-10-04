#include "stdafx.h"
#include "TexWizard.h"
#ifdef GAME_UG
#include "UG_Address.h"
#elif GAME_UG2
#include "UG2_Address.h"
#elif GAME_MW
#include "MW_Address.h"
#elif GAME_CARBON
#include "Carbon_Address.h"
#elif GAME_PS
#include "PS_Address.h"  
#elif GAME_UC
#include "UC_Address.h"  
#endif

// DllMain function
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        {
            // Initialize the D3D device
            InitD3DDevice();

            // Initialize addresses based on the game
#ifdef GAME_UG
            InitUGAddresses();  // Ensure this calls InitBaseAddresses inside
#elif GAME_UG2
            InitUG2Addresses();
#elif GAME_MW
            InitMWAddresses();
#elif GAME_CARBON
            InitCARBONAddresses();
#elif GAME_PS
            InitPSAddresses();
#elif GAME_UC
            InitUCAddresses();
#endif
            
            // Get the game folder path
            std::string gameFolderPath = GetGameFolderPath();

            // Load texture mappings and bind textures asynchronously
            std::string texturePacksPath = gameFolderPath + "\\NextGenGraphics\\TexturePacks\\";
            LoadTextureMappings(texturePacksPath.c_str());
            LoadAndBindTexturesAsync(texturePacksPath);

            // Hook the original LoadTexture function
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
