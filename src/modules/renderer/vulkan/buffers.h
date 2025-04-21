#ifndef RENDERER_VULKAN_BUFFERS
#define RENDERER_VULKAN_BUFFERS

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>
#include <cstdint>

#include <vma/vk_mem_alloc.h>

#include "mesh/mesh.h"

namespace buffers {
	VmaAllocator createVmaAllocator(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance);

	class vertexBuffer {
	public:
		vertexBuffer(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VmaAllocator allocator, VkDeviceSize initialCapacity = 1024 * 1024);
		~vertexBuffer();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
			VkBuffer* buffer, VmaAllocation* allocation,
			VmaMemoryUsage memoryUsage);
		void copyBufferRegion(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
			VkDeviceSize srcOffset, VkDeviceSize dstOffset);
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void ensureCapacity(VkDeviceSize requiredSize);

		VkCommandBuffer beginSingleTimeCommands();
		size_t appendVertices(const std::vector<vertex>& newVertices);

		VmaAllocator getVmaAllocator() const { return allocator; }
		VkBuffer getVertexBuffer() const { return _vertexBuffer; }

	private:
		VkDevice device;
		VmaAllocator allocator;
		VkQueue graphicsQueue;
		VkCommandPool commandPool;

		VkBuffer _vertexBuffer;
		VmaAllocation allocation;
		VkDeviceSize capacity;
		VkDeviceSize used;
	};

	class indexBuffer {
	public:
		indexBuffer(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VmaAllocator allocator, VkDeviceSize initialCapacity = 1024 * 1024);
		~indexBuffer();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
			VkBuffer* buffer, VmaAllocation* allocation,
			VmaMemoryUsage memoryUsage);
		void copyBufferRegion(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
			VkDeviceSize srcOffset, VkDeviceSize dstOffset);
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void ensureCapacity(VkDeviceSize requiredSize);

		VkCommandBuffer beginSingleTimeCommands();
		size_t appendIndices(const std::vector<uint32_t>& newIndices);

		VmaAllocator getVmaAllocator() const { return allocator; }
		VkBuffer getIndexBuffer() const { return _indexBuffer; }

	private:
		VkDevice device;
		VmaAllocator allocator;
		VkQueue graphicsQueue;
		VkCommandPool commandPool;

		VkBuffer _indexBuffer;
		VmaAllocation allocation;
		VkDeviceSize capacity;
		VkDeviceSize used;
	};
}

#endif // RENDERER_VULKAN_BUFFERS