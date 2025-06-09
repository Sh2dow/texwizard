#pragma once

struct TexturePack;

class TexturePackLoader
{
public:
    // Loads all texture packs from NextGenGraphics/TexturePacks/
    void LoadAllPacks();
    void LoadTexturePack(const std::filesystem::path& packPath);
    void MarkTexturesForRebind();
    // Rebinds the specified texture if the game is in the FrontEndSafeZone
    void BindTextureIfSafe(uint32_t bindHash, IDirect3DTexture9* texture);
    void RebindAllAvailableTextures();
    // Returns whether it is currently safe to bind custom textures (based on game state)
    bool IsFrontendSafeZone();
    IDirect3DTexture9* LoadTextureFromFile(const std::string& path);

private:
    // uint32_t ComputeBindHash(const std::string& texturePath);
    uint32_t ComputeBindHash(const std::string& input);

    std::unordered_map<uint32_t, std::string> m_bindHashToFileMap;

    // NEW: internal maps for pack management
    std::unordered_map<std::string, std::string> m_GameIdToFileMap;
    std::unordered_map<uint32_t, std::string> m_PendingTextures;
    std::vector<TexturePack> m_LoadedTexturePacks;
    std::mutex m_PackMutex;
};


struct TextureMapping
{
    std::string gameId;
    std::string texturePath;
};

struct TexturePack
{
    std::string id;
    std::string name;
    std::string description;
    std::string author;
    std::string rootDir;
    std::vector<TextureMapping> mappings;
};

static std::vector<TexturePack> g_LoadedTexturePacks;
static std::unordered_map<std::string, std::string> g_GameIdToFileMap;
static std::unordered_map<std::string, std::string> pendingTextures;
static std::mutex g_TextureLoadMutex;
static bool g_IsFrontendSafeZone = false;

static std::unordered_map<std::string, IDirect3DTexture9**> g_TextureSlots;
static std::unordered_map<std::string, std::string> g_ReplacementPaths;
static std::mutex g_TexMutex;
