#include "vulkan_device.h"

void VulkanDevice::waitIdle()
{
	vkDeviceWaitIdle(vkDevice);
}

bool VulkanDevice::IsValid()
{
	if (vkDevice != VK_NULL_HANDLE) {
		return true;
	}

	return false;
}

VkDevice VulkanDevice::getVkHandle()
{
	return vkDevice;
}

VkDevice* VulkanDevice::getVkHandlePtr()
{
	return &vkDevice;
}
