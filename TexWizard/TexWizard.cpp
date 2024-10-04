#include "stdafx.h"
#include "TexWizard.h"

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


#include <map>
namespace fs = std::filesystem;


// Define the global variable here
IDirect3DDevice9* D3DDevice = nullptr;
OriginalLoadTextureFunc originalLoadTexture = nullptr;
std::unordered_map<unsigned int, unsigned int> textureMap;
std::unordered_map<std::string, std::future<IDirect3DTexture9*>> asyncTextureMap;
std::unordered_map<std::string, std::string> unordered_textureMap;
std::vector<char*> packList = {};

// Function to initialize the D3D device
BOOL InitD3DDevice()
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
		// Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
	{
		Init();
	}
#endif
	else
	{
		MessageBoxA(NULL, WrongEntryPointErrorString, "TexWizard", MB_ICONERROR);
		return FALSE;
	}
    // Obtain the Direct3D device from the game (implementation specific to the game)
}

// Function to get the game folder path dynamically
std::string GetGameFolderPath()
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath(path);
    std::string::size_type pos = fullPath.find_last_of("\\/");
    return (std::string::npos == pos) ? "" : fullPath.substr(0, pos);
}

// Helper function to hash texture names
unsigned int HashTextureName(const std::string& textureName)
{
    // Assuming the hashing mechanism is based on a custom function or the game's internal hashing
    return bStringHash(const_cast<char*>(textureName.c_str())); // Use bStringHash from the game
}

// Function to load a single texture asynchronously and return the loaded texture
std::future<IDirect3DTexture9*> LoadTextureAsync(const std::string& filePath, const std::string& fileName)
{
    return std::async(std::launch::async, [filePath, fileName]() -> IDirect3DTexture9* {
        unsigned int textureHash = HashTextureName(fileName);
        textureMap[textureHash] = textureHash;

        IDirect3DTexture9* texture = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(D3DDevice, filePath.c_str(), &texture);
        if (SUCCEEDED(hr))
        {
            return texture; // Return the loaded texture
        }
        return nullptr; // Return null if texture loading failed
    });
}

IDirect3DTexture9* LoadCustomTexture(const std::string& filePath)
{
    IDirect3DTexture9* texture = nullptr;
    HRESULT hr = D3DXCreateTextureFromFile(D3DDevice, filePath.c_str(), &texture);

    if (SUCCEEDED(hr))
    {
        return texture; // Return the successfully loaded texture
    }
    else
    {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return nullptr;
    }
}

DWORD* __cdecl ReplaceTexture(unsigned int hash, int returnDefault, int includeUnloadedTextures)
{
    // Check if there's a replacement for the texture hash
    if (textureMap.count(hash) > 0)
    {
        hash = textureMap[hash];
    }

    // Call the original game's function with the (possibly replaced) hash
    return GetTextureInfo(hash, returnDefault, includeUnloadedTextures);
}

// Function to traverse the texture packs folder and bind textures asynchronously
void LoadAndBindTexturesAsync(const std::string& basePath)
{
    std::string texturePacksPath = basePath + "/NextGenGraphics/TexturePacks/";
    std::vector<std::future<void>> futures;

    // Iterate through each folder in the TexturePacks directory
    for (const auto& entry : fs::directory_iterator(texturePacksPath))
    {
        if (entry.is_directory())
        {
            std::string folderPath = entry.path().string();
            std::string texturesPath = folderPath + "/Textures";

            if (fs::exists(texturesPath))
            {
                // Add the texture pack data path to the packList
                char* packPath = new char[folderPath.length() + 1];
                strcpy(packPath, folderPath.c_str());
                packList.push_back(packPath);

                // Scan through all files in the Textures folder
                for (const auto& textureFile : fs::recursive_directory_iterator(texturesPath))
                {
                    if (textureFile.is_regular_file())
                    {
                        // Get the texture file name without extension (this is your texture key)
                        std::string fileName = textureFile.path().stem().string();
                        unsigned int fileHash = HashTextureName(fileName);
                        // Use a hash function to hash the texture name

                        // Assume that fileHash maps directly to the texture (for this example)
                        // Add the texture to the map: original hash -> new hash
                        textureMap[fileHash] = fileHash; // Replace with your actual hash mapping logic
                    }
                }
            }
        }
    }

    // Ensure all texture loading is complete
    for (auto& future : futures)
    {
        future.get(); // Blocks until each texture loading is finished
    }
}

// Hooked texture loading function
IDirect3DTexture9* WINAPI HookedLoadTexture(const char* texturePath)
{
    std::string textureId(texturePath);

    if (unordered_textureMap.find(textureId) != unordered_textureMap.end())
    {
        std::string customTexturePath = unordered_textureMap[textureId];

        // Extract the file name (without extension) from the custom texture path
        std::string fileName = fs::path(customTexturePath).stem().string();

        // Check if the texture is already being loaded or has been loaded
        if (asyncTextureMap.find(customTexturePath) == asyncTextureMap.end())
        {
            // Start loading the texture asynchronously, passing both the path and file name
            asyncTextureMap[customTexturePath] = LoadTextureAsync(customTexturePath, fileName);
        }

        // Check if the texture loading is complete
        auto& future = asyncTextureMap[customTexturePath];
        if (future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
        {
            IDirect3DTexture9* texture = future.get();
            if (texture)
            {
                return texture;
            }
        }
    }

    return originalLoadTexture(texturePath);
}

// Function to load texture mappings from a given path
void LoadTextureMappings(const std::string& path)
{
    // Implement logic to load texture mappings (e.g., from a file)
    std::cout << "Loading texture mappings from: " << path << std::endl;
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

inline void Init()
{
	// Initialize addresses based on the game
#ifdef GAME_UG
	InitUGAddresses();  // Ensure this calls InitBaseAddresses inside
#elif GAME_UG2
	InitUG2Addresses();
#elif GAME_MW
	InitMWAddresses();
#elif GAME_CARBON
	InitCARBONAddresses();
#elif GAME_PS
	InitPSAddresses();
#elif GAME_UC
	InitUCAddresses();
#endif
	
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
#endif
	
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
}
