#include "uniform_buffer.h"

#include <spdlog/spdlog.h>

UniformBuffer::UniformBuffer(VmaAllocator allocator, VkDeviceSize size)
	: _allocator(allocator), bufferSize(size)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo allocationResult;
	if (vmaCreateBuffer(_allocator, &bufferInfo, &allocInfo, &buffer, &allocation, &allocationResult) != VK_SUCCESS) {
		LOG_CRITICAL("Failed to create uniform buffer (VMA)");
	}

	mappedMemory = allocationResult.pMappedData;
}

UniformBuffer::~UniformBuffer()
{
	if (mappedMemory) {
		vmaUnmapMemory(_allocator, allocation);
		mappedMemory = nullptr;
	}
	if (buffer != VK_NULL_HANDLE && allocation != nullptr) {
		vmaDestroyBuffer(_allocator, buffer, allocation);
		buffer = VK_NULL_HANDLE;
		allocation = nullptr;
	}
}

void UniformBuffer::writeToBuffer(void* data, VkDeviceSize size)
{
	if (size == VK_WHOLE_SIZE) {
		memcpy(mappedMemory, data, bufferSize);
	}
	else {
		memcpy(mappedMemory, data, size);
	}
}

VkDescriptorBufferInfo UniformBuffer::getDescriptorInfo() const
{
	VkDescriptorBufferInfo descriptorInfo{};
	descriptorInfo.buffer = buffer;
	descriptorInfo.offset = 0;
	descriptorInfo.range = bufferSize;
	return descriptorInfo;
}
