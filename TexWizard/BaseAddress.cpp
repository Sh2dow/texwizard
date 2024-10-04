#include "stdafx.h"        // Include precompiled headers
#include "BaseAddress.h"

unsigned int (*bStringHash)(char* StringToHash) = nullptr;
DWORD* (*CreateResourceFile)(int a1, int a2, int a3, int a4, int a5) = nullptr;
int (*ResourceFileBeginLoading)(DWORD* r, int unk1, int unk2) = nullptr;
int (*LoadGlobalChunks)() = nullptr;
DWORD* (*SimpleGetTextureInfo)(unsigned int hash) = nullptr;
DWORD* (*GetTextureInfo)(unsigned int hash, int returnDefault, int includeUnloadedTextures) = nullptr;

void InitBaseAddresses(unsigned int bStringHashAddr, unsigned int CreateResourceFileAddr,
                       unsigned int ResourceFileBeginLoadingAddr, unsigned int LoadGlobalChunksAddr,
                       unsigned int GetTextureInfoAddr) {
    bStringHash = (unsigned int(*)(char*))bStringHashAddr;
    CreateResourceFile = (DWORD* (*)(int, int, int, int, int))CreateResourceFileAddr;
    ResourceFileBeginLoading = (int(*)(DWORD*, int, int))ResourceFileBeginLoadingAddr;
    LoadGlobalChunks = (int(*)())LoadGlobalChunksAddr;
    SimpleGetTextureInfo = (DWORD* (*)(unsigned int))GetTextureInfoAddr;
    GetTextureInfo = (DWORD* (*)(unsigned int, int, int))GetTextureInfoAddr;
}
