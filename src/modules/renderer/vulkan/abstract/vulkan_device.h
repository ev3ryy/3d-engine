#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>
#include <irenderer.h>

class VulkanDevice : public IDevice {
public:
	VulkanDevice(VkDevice device) : vkDevice(device) {}

	void waitIdle() override;
	bool IsValid() override;

	VkDevice getVkHandle();
	VkDevice* getVkHandlePtr();

private:
	VkDevice vkDevice;
};

#endif // VULKAN_DEVICE_H