#include "vulkan_shader.h"

#include <logs.h>
#include <fstream>

VkShaderStageFlagBits VulkanShader::toVkShaderStage(ShaderStage stage) {
    switch (stage) {
    case ShaderStage::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
    case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
    case ShaderStage::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
    default:
        LOG_CRITICAL("Unknown shader stage!");
        return static_cast<VkShaderStageFlagBits>(0);
    }
}

VulkanShader::VulkanShader(VkDevice device, const ShaderBlobSet& blobs) :
    device(device)
{
    for (const auto& blob : blobs) {
        if (blob.code.empty()) {
            LOG_WARN("Skipping empty shader blob for stage");
            continue;
        }

        VulkanShaderModule mod;
        mod.entryPointName = blob.entryPoint;
        mod.module = createShaderModule(blob);
        mod.stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        mod.stageInfo.stage = toVkShaderStage(blob.stage);
        mod.stageInfo.module = mod.module;

        modules.push_back(std::move(mod));
    }

    for (auto& mod : modules) {
        mod.stageInfo.pName = mod.entryPointName.c_str();
    }

    for (const auto& mod : modules) {
        shaderStages.push_back(mod.stageInfo);
    }

    if (shaderStages.empty()) {
        LOG_CRITICAL("VulkanShader created without any valid shader stages!");
    }
}

VulkanShader::~VulkanShader() {
    for (const auto& mod : modules) {
        if (mod.module != VK_NULL_HANDLE)
            vkDestroyShaderModule(device, mod.module, nullptr);
    }
    modules.clear();
}

VkShaderModule VulkanShader::createShaderModule(const ShaderBlob& blob) {
    if (blob.code.empty()) {
        LOG_CRITICAL("Attempted to create Vulkan module from empty shader code!");
    }

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = blob.code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(blob.code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        LOG_CRITICAL("Failed to create Vulkan shader module!");
    }
    return shaderModule;
}