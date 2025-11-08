#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include <vulkan/vulkan.h>

#include "i_shader.h"

#include <vector>
#include <string>

struct VulkanShaderModule {
	VkShaderModule module = VK_NULL_HANDLE;
	VkPipelineShaderStageCreateInfo stageInfo = {};

	std::string entryPointName;
};

class VulkanShader final : public IShader {
public:
	VulkanShader(VkDevice device, const ShaderBlobSet& blobs);
	~VulkanShader() override;


	const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const {
		return shaderStages;
	}

private:
	VkDevice device;

	std::vector<VulkanShaderModule> modules;
	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

	VkShaderModule createShaderModule(const ShaderBlob& blob);
	VkShaderStageFlagBits toVkShaderStage(ShaderStage stage);
};

#endif // VULKAN_SHADER_H