#pragma once
#include <map>

inline std::vector<char*> packList = {};
inline std::map<unsigned int, unsigned int> textureMap = {};

DWORD* __cdecl ReplaceTexture(unsigned int hash, int returnDefault, int includeUnloadedTextures);
int __fastcall LoadPacks();
void Init();
