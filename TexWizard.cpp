#include "stdafx.h"
#include "TexWizard.h"

DWORD* __cdecl ReplaceTexture(unsigned int hash, int returnDefault, int includeUnloadedTextures)
{
    if (textureMap.count(hash) > 0)
    {
        hash = textureMap[hash];
    }

    return GetTextureInfo(hash, returnDefault, includeUnloadedTextures);
}

int __fastcall LoadPacks()
{
#ifdef GAME_UC
    	int result = LoadGlobalAChunks();
#else
    int result = LoadGlobalChunks();
#endif


    // https://github.com/xan1242/xnfsmodfiles
    for (int index = 0; index < packList.size(); index++)
    {
        DWORD* r = CreateResourceFile((int)packList[index], 1, 0, 0, 0);
#ifdef GAME_UC
    		r[10] = 0x2000;
    		r[11] = *(int*)0xD3BDD4;
    		r[9] = SharedStringPoolAllocate(packList[index]);
#endif
        ResourceFileBeginLoading(r, 0, 0);
    }

    return result;
}

void LoadGlobalAChunks()
{
#ifdef GAME_UC
	// replace LoadGlobalAChunks call
	injector::MakeJMP(LoadGlobalAChunks_Hook_Addr_1, LoadPacks, true);
#else
    // replace LoadGlobalChunks call
    injector::MakeCALL(LoadGlobalChunks_Hook_Addr_1, LoadPacks, true);
#endif

    // replace all GetTextureInfo calls
#ifdef GAME_UC
	injector::MakeJMP(GetTextureInfo_Hook_Addr_1J, ReplaceTexture, true);
#else
    injector::MakeCALL(GetTextureInfo_Hook_Addr_1, ReplaceTexture, true);
#endif
    injector::MakeCALL(GetTextureInfo_Hook_Addr_2, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_3, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_4, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_5, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_6, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_7, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_8, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_9, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_10, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_11, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_12, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_13, ReplaceTexture, true);

#ifndef GAME_UC
    injector::MakeCALL(GetTextureInfo_Hook_Addr_14, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_15, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_16, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_17, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_18, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_19, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_20, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_21, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_22, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_23, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_24, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_25, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_26, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_27, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_28, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_29, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_30, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_31, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_32, ReplaceTexture, true);
#ifdef GAME_PS
	injector::MakeJMP(GetTextureInfo_Hook_Addr_33J, ReplaceTexture, true);
	injector::MakeJMP(GetTextureInfo_Hook_Addr_34J, ReplaceTexture, true);
#else
    injector::MakeCALL(GetTextureInfo_Hook_Addr_33, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_34, ReplaceTexture, true);
#endif
    injector::MakeCALL(GetTextureInfo_Hook_Addr_35, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_36, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_37, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_38, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_39, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_40, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_41, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_42, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_43, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_44, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_45, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_46, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_47, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_48, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_49, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_50, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_51, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_52, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_53, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_54, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_55, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_56, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_57, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_58, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_59, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_60, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_61, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_62, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_63, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_64, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_65, ReplaceTexture, true);

#if (defined (GAME_PS) ||defined (GAME_CARBON) || defined (GAME_MW) || defined (GAME_UG2))
    injector::MakeCALL(GetTextureInfo_Hook_Addr_66, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_67, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_68, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_69, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_70, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_71, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_72, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_73, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_74, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_75, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_76, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_77, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_78, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_79, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_80, ReplaceTexture, true);
    injector::MakeCALL(GetTextureInfo_Hook_Addr_81, ReplaceTexture, true);
#endif

#if (defined (GAME_PS) ||defined (GAME_CARBON))
	
	injector::MakeCALL(GetTextureInfo_Hook_Addr_82, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_83, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_84, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_85, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_86, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_87, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_88, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_89, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_90, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_91, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_92, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_93, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_94, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_95, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_96, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_97, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_98, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_99, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_100, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_101, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_102, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_103, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_104, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_105, ReplaceTexture, true);
	
#endif

#ifdef GAME_PS
	injector::WriteMemory(GetTextureInfo_Hook_Addr_106P, &ReplaceTexture, true);
	injector::WriteMemory(GetTextureInfo_Hook_Addr_107P, &ReplaceTexture, true);
	injector::MakeJMP(GetTextureInfo_Hook_Addr_108J, ReplaceTexture, true);
	injector::MakeJMP(GetTextureInfo_Hook_Addr_109J, ReplaceTexture, true);
#endif

#ifdef GAME_CARBON

	injector::MakeCALL(GetTextureInfo_Hook_Addr_106, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_107, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_108, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_109, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_110, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_111, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_112, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_113, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_114, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_115, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_116, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_117, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_118, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_119, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_120, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_121, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_122, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_123, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_124, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_125, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_126, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_127, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_128, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_129, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_130, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_131, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_132, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_133, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_134, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_135, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_136, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_137, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_138, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_139, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_140, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_141, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_142, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_143, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_144, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_145, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_146, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_147, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_148, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_149, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_150, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_151, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_152, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_153, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_154, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_155, ReplaceTexture, true);
	injector::MakeCALL(GetTextureInfo_Hook_Addr_156, ReplaceTexture, true);

#endif

#endif
}


void LoadNGGPacks()
{
    fs::path baseDir = "NextGenGraphics/TexturePacks";
    if (!fs::exists(baseDir)) return;

    for (const auto& dir : fs::directory_iterator(baseDir))
    {
        if (!fs::is_directory(dir)) continue;

        fs::path infoPath = dir.path() / "TexturePackInfo.json";
        if (!fs::exists(infoPath)) continue;

        std::ifstream file(infoPath);
        if (!file.is_open()) continue;

        json info;
        try
        {
            file >> info;
        }
        catch (...)
        {
            continue;
        }

        std::string root = info.value("rootDirectory", "Textures");

        for (const auto& mapping : info["textureMappings"])
        {
            std::string texturePath = mapping.value("texturePath", "");
            if (texturePath.empty()) continue;

            fs::path textureStem = fs::path(texturePath).stem();
            if (textureStem.empty()) continue;

            std::string rawGameId = mapping.value("gameId", textureStem.string());
            fs::path fullPath = dir.path() / root / texturePath;
            if (!fs::exists(fullPath)) continue;

            // Sanitize gameId
            std::string gameId;
            for (char c : rawGameId)
                if (c != '_')
                    gameId += std::tolower(static_cast<unsigned char>(c));
            if (gameId.empty()) continue;

            uint32_t hash = bStringHash((char*)gameId.c_str());

            uint32_t ptrHash = 0;
            try
            {
                ptrHash = std::stoul(textureStem.string(), nullptr, 16);
            }
            catch (...)
            {
                continue;
            }

            textureMap[hash] = ptrHash;

            printf("[TexWizard] Registered: 0x%08X -> 0x%08X (%s)\n", hash, ptrHash, gameId.c_str());
        }
    }

    // Also register raw fallback DDS files
    for (const auto& dir : fs::recursive_directory_iterator(baseDir))
    {
        fs::path texDir = dir.path() / "Textures";
        if (!fs::exists(texDir)) continue;

        for (const auto& entry : fs::recursive_directory_iterator(texDir))
        {
            if (!entry.is_regular_file() || entry.path().extension() != ".dds") continue;

            std::string name = entry.path().stem().string();
            if (name.length() < 6 || name.length() > 8 ||
                name.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos)
                continue;

            uint32_t hash = 0;
            try
            {
                hash = std::stoul(name, nullptr, 16);
            }
            catch (...)
            {
                continue;
            }

            textureMap[hash] = hash;

            printf("[TexWizard] Fallback bind: 0x%08X\n", hash);
        }
    }
}

void Init()
{
    LoadNGGPacks();
    // LoadGlobalAChunks();
}
