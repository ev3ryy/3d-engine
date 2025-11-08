#ifndef SHADER_KEY_H
#define SHADER_KEY_H

#include <shaders/i_shader.h>
#include <renderer_data.h>

#include <sstream>

struct ShaderKey {
    std::string baseName;
    ShaderStage stage;
    API_TYPE apiType;

    uint32_t flags;

    std::string toCacheKey() const {
        std::stringstream ss;
        ss << baseName << "_"
            << (stage == ShaderStage::Vertex ? "vert" : (stage == ShaderStage::Fragment ? "frag" : "comp")) << "_";

        std::string apiName = "UnknownAPI";
        std::string extension = ".blob";

        if (apiType == API_TYPE::Vulkan) {
            apiName = "Vulkan";
            extension = ".spv";
        }
        else if (apiType == API_TYPE::DX12) {
            apiName = "D3D12";
            extension = ".dxil";
        }

        ss << apiName << "_" << flags << extension;
        return ss.str();
    }
};

#endif // SHADER_KEY_H