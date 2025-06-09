#include "stdafx.h"
#include <windows.h>
#include "TexWizard.h"
#include "TexturePackLoader.h"
#include "TextureOverrideManager.h"

TexturePackLoader g_TexturePackLoader;

// Forward declaration
typedef HRESULT (WINAPI*PresentFn)(IDirect3DDevice9*, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
PresentFn OriginalPresent = nullptr;

HRESULT WINAPI HookedPresent(IDirect3DDevice9* device, const RECT* src, const RECT* dst, HWND wnd, const RGNDATA* dirty)
{
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;

        if (device && !IsBadReadPtr(device, 4))
            D3DDevice = device;

        // Load all texture packs and rebind once on startup
        g_TexturePackLoader.LoadAllPacks(); // Parse JSON and map hashes
        g_TexturePackLoader.MarkTexturesForRebind(); // Push all hashes to pendingTextures
        g_TexturePackLoader.RebindAllAvailableTextures(); // Actually bind them

        printf_s("[XNFS-TexWizard] Texture packs loaded and bound.\n");
    }

    return OriginalPresent(device, src, dst, wnd, dirty);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        {
            uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
            IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
            IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

#ifdef GAME_PS
        if (strstr((const char*)(base + (0xA49742 - base)), "ProStreet08Release.exe"))
        {
            Init();
        }
#else
            if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == EntryPoint)
            {
                Init();
            }
#endif

            DisableThreadLibraryCalls(hModule);
            InitTextureOverrideHooks();

            g_TexturePackLoader.LoadAllPacks();
            g_TexturePackLoader.MarkTexturesForRebind();

            std::thread([]()
            {
                Sleep(500);

                IDirect3DDevice9* tempDevice = nullptr;
                D3DPRESENT_PARAMETERS d3dpp = {};
                d3dpp.Windowed = TRUE;
                d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

                IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
                if (!d3d) return;

                HWND hwnd = GetForegroundWindow();
                d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &tempDevice);

                if (tempDevice)
                {
                    void** vtable = *reinterpret_cast<void***>(tempDevice);
                    OriginalPresent = (PresentFn)vtable[17];

                    DetourTransactionBegin();
                    DetourUpdateThread(GetCurrentThread());
                    DetourAttach(&(PVOID&)OriginalPresent, HookedPresent);
                    DetourTransactionCommit();

                    tempDevice->Release();
                }

                d3d->Release();
            }).detach();

            break;
        }

    case DLL_PROCESS_DETACH:
        {
            if (OriginalPresent)
            {
                DetourTransactionBegin();
                DetourUpdateThread(GetCurrentThread());
                DetourDetach(&(PVOID&)OriginalPresent, HookedPresent);
                DetourTransactionCommit();
            }
            break;
        }
    }

    return TRUE;
}
