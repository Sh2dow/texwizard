// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <future>
#include <mutex>
#include <d3d9.h>
#include <d3dx9tex.h>
// TODO: reference additional headers your program requires here
#include <nlohmann/json.hpp>
#include "./Modules/injector/include/injector/assembly.hpp"
#include "./Modules/injector/include/injector/injector.hpp"
#include "./Modules/IniReader/IniReader.h"
#include "./Modules/minhook/include/MinHook.h"
#include "./Modules/json/include/nlohmann/detail/meta/std_fs.hpp"

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

#if _DEBUG
#include "Log.h"
#define printf_s(...) asi_log::Log(__VA_ARGS__)
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

constexpr uintptr_t GLOBAL_D3DDEVICE = 0x00982BDC;
inline LPDIRECT3DDEVICE9& g_Device = *(LPDIRECT3DDEVICE9*)GLOBAL_D3DDEVICE;

inline LPDIRECT3DDEVICE9 GetGameDevice()
{
    return *(LPDIRECT3DDEVICE9*)GLOBAL_D3DDEVICE;
}

inline void SetGameDevice(LPDIRECT3DDEVICE9 device)
{
    if (device && !IsBadReadPtr(device, 4))
        g_Device = device;
}

