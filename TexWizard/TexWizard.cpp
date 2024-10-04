#include "stdafx.h"
#include "TexWizard.h"

#include "BaseAddress.h"

namespace fs = std::filesystem;

// Define the global variable here
IDirect3DDevice9* D3DDevice = nullptr;
OriginalLoadTextureFunc originalLoadTexture = nullptr;
std::unordered_map<unsigned int, unsigned int> textureMap;
std::unordered_map<std::string, std::future<IDirect3DTexture9*>> asyncTextureMap;
std::unordered_map<std::string, std::string> unordered_textureMap;

// Function to initialize the D3D device
void InitD3DDevice() {
    // Obtain the Direct3D device from the game (implementation specific to the game)

// #ifdef GAME_UGs
//     InitUGAddresses();
// #elif GAME_UG2
//     InitUG2Addresses();
// #elif GAME_MW
//     InitMWAddresses();
// #elif GAME_CARBON
//     InitCARBONAddresses();
// #elif GAME_PS
//     InitPSAddresses();
// #elif GAME_UC
//     InitUCAddresses();
// #endif
}

// Function to get the game folder path dynamically
std::string GetGameFolderPath() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string fullPath(path);
    std::string::size_type pos = fullPath.find_last_of("\\/");
    return (std::string::npos == pos) ? "" : fullPath.substr(0, pos);
}

// Helper function to hash texture names
unsigned int HashTextureName(const std::string& textureName) {
    // Assuming the hashing mechanism is based on a custom function or the game's internal hashing
    return bStringHash(const_cast<char*>(textureName.c_str()));  // Use bStringHash from the game
}

// Function to load a single texture asynchronously and return the loaded texture
std::future<IDirect3DTexture9*> LoadTextureAsync(const std::string& filePath, const std::string& fileName) {
    return std::async(std::launch::async, [filePath, fileName]() -> IDirect3DTexture9* {
        unsigned int textureHash = HashTextureName(fileName);
        textureMap[textureHash] = textureHash;

        IDirect3DTexture9* texture = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(D3DDevice, filePath.c_str(), &texture);
        if (SUCCEEDED(hr)) {
            return texture;  // Return the loaded texture
        }
        return nullptr;  // Return null if texture loading failed
    });
}

// Function to traverse the texture packs folder and bind textures asynchronously
void LoadAndBindTexturesAsync(const std::string& basePath) {
    std::string texturePacksPath = basePath + "/NextGenGraphics/TexturePacks/";
    std::vector<std::future<IDirect3DTexture9*>> futures;

    for (const auto& entry : fs::directory_iterator(texturePacksPath)) {
        if (entry.is_directory()) {
            std::string texturesFolderPath = entry.path().string() + "/Textures/";

            if (fs::exists(texturesFolderPath)) {
                for (const auto& textureFile : fs::recursive_directory_iterator(texturesFolderPath)) {
                    if (textureFile.is_regular_file()) {
                        std::string filePath = textureFile.path().string();
                        std::string fileName = textureFile.path().stem().string();
                        futures.push_back(LoadTextureAsync(filePath, fileName));
                    }
                }
            }
        }
    }

    // Wait for all async texture loading operations to complete
    for (auto& future : futures) {
        future.get();
    }
}

// Hooked texture loading function
IDirect3DTexture9* WINAPI HookedLoadTexture(const char* texturePath) {
    std::string textureId(texturePath);

    if (unordered_textureMap.find(textureId) != unordered_textureMap.end()) {
        std::string customTexturePath = unordered_textureMap[textureId];

        // Extract the file name (without extension) from the custom texture path
        std::string fileName = fs::path(customTexturePath).stem().string();

        // Check if the texture is already being loaded or has been loaded
        if (asyncTextureMap.find(customTexturePath) == asyncTextureMap.end()) {
            // Start loading the texture asynchronously, passing both the path and file name
            asyncTextureMap[customTexturePath] = LoadTextureAsync(customTexturePath, fileName);
        }

        // Check if the texture loading is complete
        auto& future = asyncTextureMap[customTexturePath];
        if (future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            IDirect3DTexture9* texture = future.get();
            if (texture) {
                return texture;
            }
        }
    }

    return originalLoadTexture(texturePath);
}

// Function to load texture mappings from a given path
void LoadTextureMappings(const std::string& path) {
    // Implement logic to load texture mappings (e.g., from a file)
    std::cout << "Loading texture mappings from: " << path << std::endl;
}
