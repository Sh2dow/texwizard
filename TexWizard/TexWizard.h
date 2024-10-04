#pragma once
#include "stdafx.h"
#include "BaseAddress.h"

// Declare global variables (extern to avoid redefinition)
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

