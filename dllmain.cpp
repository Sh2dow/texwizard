#include "stdafx.h"
#include "TexWizard.h"
//
// #pragma comment(lib, "d3d9.lib")
// #pragma comment(lib, "d3dx9.lib")


bool g_TexWizardInitialized = false;

using PresentFn = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
PresentFn g_OriginalPresent = nullptr;

HRESULT (__stdcall*OriginalPresent)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*) = nullptr;

struct CustomTexturesHook
{
    void operator()(injector::reg_pack& regs)
    {
        // Don't use [esp + 0xC]; instead, read the bindHash from ECX (or regs.ecx)
        uint32_t& bindHash = (uint32_t&)regs.ecx;

        auto it = textureMap.find(bindHash);
        if (it != textureMap.end())
            bindHash = it->second;
    }
};


HRESULT __stdcall HookedPresent(IDirect3DDevice9* pDevice, const RECT* src, const RECT* dest, HWND wnd,
                                const RGNDATA* dirty)
{
    if (!g_TexWizardInitialized)
    {
        g_TexWizardInitialized = true;

        // Parse JSON / .dds textures
        Init(); // 🔥 Safe to load files, JSON, do anything now

        // Hook texture binding hash
        injector::MakeInline<CustomTexturesHook>(0x61D830);

        printf("[TexWizard] Hook installed at 0x61D830\n");
    }

    return g_OriginalPresent(pDevice, src, dest, wnd, dirty);
}

void InstallPresentHook()
{
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return;

    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.hDeviceWindow = GetForegroundWindow();

    IDirect3DDevice9* dummyDevice = nullptr;
    if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDevice)))
    {
        d3d->Release();
        return;
    }

    void** vTable = *reinterpret_cast<void***>(dummyDevice);
    g_OriginalPresent = (PresentFn)vTable[17];

    DWORD oldProtect;
    VirtualProtect(&vTable[17], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
    vTable[17] = (void*)&HookedPresent;
    VirtualProtect(&vTable[17], sizeof(void*), oldProtect, &oldProtect);

    dummyDevice->Release();
    d3d->Release();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
        IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

#ifdef GAME_PS
        if (strstr((const char*)(base + (0xA49742 - base)), "ProStreet08Release.exe"))
        {
            DisableThreadLibraryCalls(hModule);
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InstallPresentHook, nullptr, 0, nullptr);
        }
#else
        if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == EntryPoint)
        {
            DisableThreadLibraryCalls(hModule);
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InstallPresentHook, nullptr, 0, nullptr);
        }
#endif
        else
        {
            MessageBoxA(NULL, WrongEntryPointErrorString, "TexWizard", MB_ICONERROR);
            return FALSE;
        }
    }
    return TRUE;
}
