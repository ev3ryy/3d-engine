#ifndef RENDERER_UNIFORM_BUFFER_H
#define RENDERER_UNIFORM_BUFFER_H

#include <vk_mem_alloc.h>
#include <memory>

class UniformBuffer {
public:
	UniformBuffer(VmaAllocator allocator, VkDeviceSize size);
	~UniformBuffer();

	UniformBuffer(const UniformBuffer&) = delete;
	UniformBuffer& operator=(const UniformBuffer&) = delete;

	void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE);

	VkBuffer getBuffer() const { return buffer; }
	VkDescriptorBufferInfo getDescriptorInfo() const;

private:
	VmaAllocator _allocator;
	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = nullptr;
	void* mappedMemory = nullptr;
	VkDeviceSize bufferSize;
};

#endif // RENDERER_UNIFORM_BUFFER_H