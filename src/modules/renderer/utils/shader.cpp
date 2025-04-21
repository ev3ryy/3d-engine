#include "shader.h"

#include <spdlog/spdlog.h>
#include <fstream>

namespace utils {
    shader::shader(VkDevice device, const std::string& vertPath, const std::string& fragPath) :
        device(device)
    {
        auto vertShaderCode = readFile(vertPath);
        auto fragShaderCode = readFile(fragPath);

        vertexModule = createShaderModule(vertShaderCode);
        fragmentModule = createShaderModule(fragShaderCode);

        VkPipelineShaderStageCreateInfo vertStageInfo{};
        vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStageInfo.module = vertexModule;
        vertStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragStageInfo{};
        fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStageInfo.module = fragmentModule;
        fragStageInfo.pName = "main";

        shaderStages = { vertStageInfo, fragStageInfo };
    }

    shader::~shader()
    {
        if (vertexModule != VK_NULL_HANDLE)
            vkDestroyShaderModule(device, vertexModule, nullptr);
        if (fragmentModule != VK_NULL_HANDLE)
            vkDestroyShaderModule(device, fragmentModule, nullptr);
    }

    std::vector<char> shader::readFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            LOG_CRITICAL("failed to open file");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }

    VkShaderModule shader::createShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            LOG_CRITICAL("failed to create shader module");
        }
        return shaderModule;
    }
}
