#ifndef UTILS_SHADER_H
#define UTILS_SHADER_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

namespace utils {
	class shader {
	public:
		shader(VkDevice device, const std::string& vertPath, const std::string& fragPath);
		~shader();

		static std::vector<char> readFile(const std::string& filename);

		const std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages() const {
			return shaderStages;
		}

	private:
		VkDevice device;
		VkShaderModule vertexModule = VK_NULL_HANDLE;
		VkShaderModule fragmentModule = VK_NULL_HANDLE;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		VkShaderModule createShaderModule(const std::vector<char>& code);
	};
}

#endif // UTILS_SHADER_H