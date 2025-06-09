#pragma once
#include "stdafx.h"
#include <map>
#include "Base_Game_Address.h"

#ifdef GAME_UG
#include "UG_Address.h"
#endif

#ifdef GAME_UG2
#include "UG2_Address.h"
#endif

#ifdef GAME_MW
#include "MW_Address.h"
#endif

#ifdef GAME_CARBON
#include "Carbon_Address.h"
#endif

#ifdef GAME_PS
#include "PS_Address.h"
#endif

#ifdef GAME_UC
#include "UC_Address.h"
#endif

// Declare global variables (extern to avoid redefinition)
inline IDirect3DDevice9* D3DDevice = nullptr;

typedef IDirect3DTexture9* (WINAPI*OriginalLoadTextureFunc)(const char* texturePath);
extern OriginalLoadTextureFunc originalLoadTexture;

// Declare shared maps for texture management
extern std::unordered_map<unsigned int, unsigned int> textureMap;
extern std::unordered_map<std::string, std::future<IDirect3DTexture9*>> asyncTextureMap;
extern std::unordered_map<std::string, std::string> unordered_textureMap;

// Declare functions
void Init();
BOOL InitD3DDevice();
std::string GetGameFolderPath();
void LoadAndBindTexturesAsync(const std::string& basePath);
void LoadTextureMappings(const std::string& path);
unsigned int HashTextureName(const std::string& textureName);
IDirect3DTexture9* WINAPI HookedLoadTexture(const char* texturePath);

