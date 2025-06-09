#include "stdafx.h"
#include "TexturePackLoader.h"
#include "TexWizard.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
// #include "json/json.h" // Assume JsonCPP or similar lib
#include <nlohmann/json.hpp>
// #include "../Modules/json/include/nlohmann/json.hpp"

#ifdef GAME_MW
#include "MW_Address.h"
#endif

using json = nlohmann::json;
namespace fs = std::filesystem;

bool TexturePackLoader::IsFrontendSafeZone()
{
    const void* ptr = reinterpret_cast<void*>(0x650822CF); // NOT A8!
    MEMORY_BASIC_INFORMATION mbi = {};
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)))
    {
        if (mbi.State == MEM_COMMIT && (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READWRITE)))
        {
            return *(bool*)ptr;
        }
    }

    return false;
}

std::string DetectGameIdFromPath(const std::string& path)
{
    // Extract just the filename without extension
    size_t slash = path.find_last_of("/\\");
    std::string file = (slash != std::string::npos) ? path.substr(slash + 1) : path;

    size_t dot = file.find_last_of('.');
    return (dot != std::string::npos) ? file.substr(0, dot) : file;
}

void TexturePackLoader::LoadTexturePack(const fs::path& packPath)
{
    fs::path infoPath = packPath / "TexturePackInfo.json";
    if (!fs::exists(infoPath)) return;

    std::ifstream f(infoPath);
    nlohmann::json root;
    f >> root;

    TexturePack pack;
    pack.id = root["description"]["id"].get<std::string>();
    pack.name = root["description"]["name"].get<std::string>();
    pack.description = root["description"]["description"].get<std::string>();
    pack.author = root["description"]["author"].get<std::string>();
    pack.rootDir = root["rootDirectory"].get<std::string>();

    for (const auto& mapping : root["textureMappings"])
    {
        TextureMapping texMap;
        texMap.gameId = mapping["gameId"].get<std::string>();
        texMap.texturePath = mapping["texturePath"].get<std::string>();
        pack.mappings.push_back(texMap);

        // Full path resolution
        fs::path fullTexPath = packPath / pack.rootDir / texMap.texturePath;
        if (fs::exists(fullTexPath))
        {
            std::lock_guard<std::mutex> lock(m_PackMutex);
            g_GameIdToFileMap[texMap.gameId] = fullTexPath.string();
        }
    }

    g_LoadedTexturePacks.push_back(std::move(pack));
}

// uint32_t bStringHash(const char* str)
// {
//     uint32_t hash = 0;
//     while (*str)
//     {
//         hash = (hash >> 0x1D) + (hash << 5) + *str;
//         ++str;
//     }
//     return hash;
// }

uint32_t TexturePackLoader::ComputeBindHash(const std::string& input)
{
    return bStringHash(const_cast<char*>(input.c_str())); // Use Binary's hash directly
}

std::string PrepareTextureNameForHash(const std::string& fullPath)
{
    std::string normalized = fullPath;
    std::replace(normalized.begin(), normalized.end(), '\\', '/'); // Normalize slashes

    const std::string marker = "Textures/";
    size_t pos = normalized.find(marker);
    if (pos != std::string::npos)
    {
        normalized = normalized.substr(pos + marker.length()); // Remove "Textures/"
    }

    fs::path np(normalized);
    return np.replace_extension("").string(); // Remove extension
}

void TexturePackLoader::LoadAllPacks()
{
    fs::path baseDir = "NextGenGraphics/TexturePacks";
    if (!fs::exists(baseDir)) return;

    for (const auto& dir : fs::directory_iterator(baseDir))
    {
        if (!fs::is_directory(dir)) continue;

        fs::path infoPath = dir.path() / "TexturePackInfo.json";
        if (!fs::exists(infoPath)) continue;

        std::ifstream file(infoPath);
        json info;
        file >> info;

        std::string root = info.value("rootDirectory", "Textures");

        for (const auto& mapping : info["textureMappings"])
        {
            std::string texturePath = mapping["texturePath"];
            std::string gameId = mapping.value("gameId", fs::path(texturePath).stem().string());
            fs::path fullPath = dir.path() / root / texturePath;

            if (!fs::exists(fullPath))
            {
                printf_s("[TexPack] Skipped missing file: %s\n", fullPath.string().c_str());
                continue;
            }

            // No lowercase or slash fixing — use raw gameId
            uint32_t hash = ComputeBindHash(gameId);

            m_bindHashToFileMap[hash] = fullPath.string();

            printf_s("[XNFS-TexWizard] [TexPack] Registered: 0x%08X -> %s (from \"%s\")\n",
                     hash, fullPath.string().c_str(), gameId.c_str());
        }
    }
}

void TexturePackLoader::MarkTexturesForRebind()
{
    std::lock_guard<std::mutex> lock(m_PackMutex);
    for (const auto& pair : m_bindHashToFileMap)
    {
        uint32_t bindHash = pair.first;
        const std::string& texPath = pair.second;

        m_PendingTextures[bindHash] = texPath;
    }
}

void TexturePackLoader::BindTextureIfSafe(uint32_t bindHash, IDirect3DTexture9* texture)
{
    if (!IsFrontendSafeZone()) return;

    auto it = m_bindHashToFileMap.find(bindHash);
    if (it == m_bindHashToFileMap.end()) return;

    const std::string& texFile = it->second;
    IDirect3DDevice9* device = GetGameDevice();
    if (!device) return;

    IDirect3DTexture9* newTex = nullptr;
    if (SUCCEEDED(D3DXCreateTextureFromFileA(device, texFile.c_str(), &newTex)) && newTex)
    {
        // TODO: Replace texture in game
        printf_s("Bound texture: %s\n", texFile.c_str());
        // Example only: newTex->Release(); if you won't keep the reference
    }
}


IDirect3DTexture9* TexturePackLoader::LoadTextureFromFile(const std::string& path)
{
    if (!D3DDevice)
    {
        printf_s("LoadTextureFromFile failed: device is null");
        return nullptr;
    }

    IDirect3DTexture9* texture = nullptr;
    HRESULT hr = D3DXCreateTextureFromFileA(D3DDevice, path.c_str(), &texture);

    if (FAILED(hr))
    {
        printf_s("D3DXCreateTextureFromFileA failed for %s (HRESULT=0x%08X)", path.c_str(), hr);
        return nullptr;
    }

    return texture;
}


void TexturePackLoader::RebindAllAvailableTextures()
{
    std::lock_guard<std::mutex> lock(m_PackMutex);

    if (m_PendingTextures.empty())
    {
        printf_s("[XNFS-TexWizard] No pending textures to rebind.\n");
        return;
    }

    printf_s("[XNFS-TexWizard] Rebinding %zu pending textures...\n", m_PendingTextures.size());

    for (const auto& [bindHash, texPath] : m_PendingTextures)
    {
        IDirect3DTexture9* newTex = nullptr;
        IDirect3DDevice9* device = GetGameDevice();
        if (!device) continue;

        HRESULT hr = D3DXCreateTextureFromFileA(device, texPath.c_str(), &newTex);
        if (FAILED(hr) || !newTex)
        {
            printf("[XNFS-TexWizard] Failed to load texture: %s\n", texPath.c_str());
            continue;
        }

        // Replace actual slot here using bindHash if applicable
        printf("[XNFS-TexWizard] Rebound texture: 0x%08X from %s\n", bindHash, texPath.c_str());
    }

    m_PendingTextures.clear();
}
