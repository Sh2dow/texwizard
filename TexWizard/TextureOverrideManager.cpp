#include "TextureOverrideManager.h"
#include <d3d9.h>
#include <detours.h>
#include <windows.h>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "stdafx.h"

static std::unordered_map<std::string, IDirect3DTexture9**> g_TextureSlots;
static std::mutex g_TexMutex;

typedef HRESULT (WINAPI*D3DXCreateTextureFromFileA_t)(
    LPDIRECT3DDEVICE9, LPCSTR, LPDIRECT3DTEXTURE9*);

static std::string GetBaseName(const std::string& path)
{
    size_t slash = path.find_last_of("/\\");
    std::string file = (slash != std::string::npos) ? path.substr(slash + 1) : path;

    size_t dot = file.find_last_of('.');
    return (dot != std::string::npos) ? file.substr(0, dot) : file;
}

// HRESULT WINAPI Hooked_D3DXCreateTextureFromFileA(
//     LPDIRECT3DDEVICE9 device, LPCSTR pSrcFile, LPDIRECT3DTEXTURE9* ppTexture)
// {
//     HRESULT hr = Real_D3DXCreateTextureFromFileA(device, pSrcFile, ppTexture);
//
//     if (SUCCEEDED(hr) && ppTexture && pSrcFile)
//     {
//         std::string gameId = GetBaseName(pSrcFile);
//
//         {
//             std::lock_guard<std::mutex> lock(g_TexMutex);
//             g_TextureSlots[gameId] = ppTexture;
//         }
//
//         // Avoid crash: check if *ppTexture is non-null before logging pointer
//         printf_s("[NGGStyle] Bound slot for %s -> %p\n", gameId.c_str(), static_cast<void*>(*ppTexture));
//     }
//
//     return hr;
// }
HRESULT WINAPI Hooked_D3DXCreateTextureFromFileA(
    LPDIRECT3DDEVICE9 device, LPCSTR pSrcFile, LPDIRECT3DTEXTURE9* ppTexture)
{
    if (!Real_D3DXCreateTextureFromFileA)
        return D3DERR_INVALIDCALL;

    HRESULT hr = Real_D3DXCreateTextureFromFileA(device, pSrcFile, ppTexture);

    if (SUCCEEDED(hr) && ppTexture && *ppTexture)
    {
        // Safe to log or store
        printf_s("[TexWizard] Hooked creation: %s => %p\n", pSrcFile, *ppTexture);
    }
    else
    {
        printf_s("[TexWizard] Texture creation failed for %s (hr=0x%08X)\n", pSrcFile, hr);
    }

    return hr;
}

void BindReplacementTexture(const std::string& gameId, const std::string& replacementPath)
{
    std::lock_guard<std::mutex> lock(g_TexMutex);

    auto it = g_TextureSlots.find(gameId);
    if (it == g_TextureSlots.end())
    {
        std::cerr << "[NGGStyle] Texture slot for " << gameId << " not found\n";
        return;
    }

    IDirect3DTexture9** pSlot = it->second;
    if (!pSlot) return;

    LPDIRECT3DDEVICE9 device = nullptr;
    if (*pSlot) (*pSlot)->GetDevice(&device);

    if (!device)
    {
        std::cerr << "[NGGStyle] Device not found for " << gameId << "\n";
        return;
    }

    IDirect3DTexture9* newTex = nullptr;
    if (SUCCEEDED(D3DXCreateTextureFromFileA(device, replacementPath.c_str(), &newTex)))
    {
        if (*pSlot)
            (*pSlot)->Release();

        *pSlot = newTex;
        newTex->AddRef();

        printf_s("[NGGStyle] Replaced texture: %gameId, %replacementPath, \n", gameId.c_str(), replacementPath.c_str());
    }

    device->Release();
}

void InitTextureOverrideHooks()
{
    HMODULE d3dx = LoadLibraryA("d3dx9_43.dll");
    if (!d3dx)
    {
        MessageBoxA(nullptr, "Failed to load d3dx9_43.dll", "TexWizard Error", MB_ICONERROR);
        return;
    }

    Real_D3DXCreateTextureFromFileA = (decltype(Real_D3DXCreateTextureFromFileA))
        GetProcAddress(d3dx, "D3DXCreateTextureFromFileA");

    if (!Real_D3DXCreateTextureFromFileA)
    {
        MessageBoxA(nullptr, "Failed to get address of D3DXCreateTextureFromFileA from d3dx9_43.dll", "TexWizard Error", MB_ICONERROR);
        return;
    }

    if (DetourTransactionBegin() != NO_ERROR ||
        DetourUpdateThread(GetCurrentThread()) != NO_ERROR ||
        DetourAttach(&(PVOID&)Real_D3DXCreateTextureFromFileA, Hooked_D3DXCreateTextureFromFileA) != NO_ERROR ||
        DetourTransactionCommit() != NO_ERROR)
    {
        MessageBoxA(nullptr, "Detour hook failed", "TexWizard Error", MB_ICONERROR);
    }
    else
    {
        printf_s("[TexWizard] Hooked D3DXCreateTextureFromFileA from d3dx9_43.dll\n");
    }
}

