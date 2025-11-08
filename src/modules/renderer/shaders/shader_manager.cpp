#include <shaders/shader_manager.h>
#include <shaders/i_shader.h>
#include <logs.h>
#include <fstream>
#include <sstream>

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        LOG_WARN("Failed to open shader file: %s ", filename.c_str());
        return {};
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

ShaderBlob ShaderManager::getBinaryBlob(const ShaderKey& key) {
    std::string cacheKey = key.toCacheKey();

    if (blobCache.count(cacheKey)) {
        return blobCache.at(cacheKey);
    }

    // example: "shaders/cache/g_buffer_vert_Vulkan_0.spv"
    std::string filename = "shaders/cache/" + cacheKey;

    std::vector<char> code = readFile(filename);

    if (code.empty()) {
        LOG_CRITICAL("Shader blob not found: %s. Must be compiled!", filename.c_str());
        // shader compil job?
        return {};
    }

    std::string entryPoint;
    switch (key.stage) {
    case ShaderStage::Vertex:
        entryPoint = "VSMain";
        break;
    case ShaderStage::Fragment:
        entryPoint = "PSMain";
        break;
    case ShaderStage::Compute:
        entryPoint = "CSMain";
        break;
    default:
        entryPoint = "main";
    }

    ShaderBlob blob = { key.stage, code, entryPoint };

    blobCache[cacheKey] = blob;
    return blob;
}