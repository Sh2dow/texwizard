#pragma once

// Declare function pointers for the external functions
extern unsigned int (*bStringHash)(char* StringToHash);
extern DWORD* (*CreateResourceFile)(int a1, int a2, int a3, int a4, int a5);
extern int (*ResourceFileBeginLoading)(DWORD* r, int unk1, int unk2);
extern int (*LoadGlobalChunks)();
extern DWORD* (*SimpleGetTextureInfo)(unsigned int textureHash);
extern DWORD* (*GetTextureInfo)(unsigned int textureHash, int returnDefault, int includeUnloadedTextures);

// Function to initialize the function pointers
void InitBaseAddresses(unsigned int bStringHashAddr, unsigned int CreateResourceFileAddr,
                       unsigned int ResourceFileBeginLoadingAddr, unsigned int LoadGlobalChunksAddr,
                       unsigned int GetTextureInfoAddr);
