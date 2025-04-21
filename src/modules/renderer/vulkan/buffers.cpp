#include "buffers.h"
#include <spdlog/spdlog.h>

VmaAllocator buffers::createVmaAllocator(VkPhysicalDevice physicalDevice, VkDevice device, VkInstance instance)
{
	VmaAllocatorCreateInfo createInfo{};
	createInfo.physicalDevice = physicalDevice;
	createInfo.device = device;
	createInfo.instance = instance;
	createInfo.flags = 0;

	VmaAllocator allocator;

	if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
		LOG_CRITICAL("failed to create VMA allocator");
	}

	return allocator;
}

buffers::vertexBuffer::vertexBuffer(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VmaAllocator allocator, VkDeviceSize initialCapacity)
	: device(device), graphicsQueue(graphicsQueue), commandPool(commandPool), allocator(allocator), capacity(initialCapacity), used(0)
{
	createBuffer(capacity,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		&_vertexBuffer, &allocation,
		VMA_MEMORY_USAGE_GPU_ONLY);
}

buffers::vertexBuffer::~vertexBuffer()
{
	vmaDestroyBuffer(allocator, _vertexBuffer, allocation);
}

void buffers::vertexBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer, VmaAllocation* allocation, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;

	if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, buffer, allocation, nullptr) != VK_SUCCESS) {
		LOG_CRITICAL("failed to create buffer with VMA");
	}
}

void buffers::vertexBuffer::copyBufferRegion(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void buffers::vertexBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void buffers::vertexBuffer::ensureCapacity(VkDeviceSize requiredSize)
{
	if (requiredSize > capacity) {
		VkDeviceSize newCapacity = std::max(capacity * 2, requiredSize);
		VkBuffer newBuffer;
		VmaAllocation newAllocation;
		createBuffer(newCapacity,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			&newBuffer, &newAllocation,
			VMA_MEMORY_USAGE_GPU_ONLY);

		copyBufferRegion(_vertexBuffer, newBuffer, used, 0, 0);

		vmaDestroyBuffer(allocator, _vertexBuffer, allocation);

		_vertexBuffer = newBuffer;
		allocation = newAllocation;
		capacity = newCapacity;
	}
}

VkCommandBuffer buffers::vertexBuffer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

size_t buffers::vertexBuffer::appendVertices(const std::vector<vertex>& newVertices)
{
	VkDeviceSize newDataSize = sizeof(vertex) * newVertices.size();
	ensureCapacity(used + newDataSize);

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	createBuffer(newDataSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		&stagingBuffer, &stagingAllocation,
		VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(allocator, stagingAllocation, &data);
	memcpy(data, newVertices.data(), static_cast<size_t>(newDataSize));
	vmaUnmapMemory(allocator, stagingAllocation);

	copyBufferRegion(stagingBuffer, _vertexBuffer, newDataSize, 0, used);

	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

	size_t offset = used;
	used += newDataSize;
	return offset;
}

buffers::indexBuffer::indexBuffer(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VmaAllocator allocator, VkDeviceSize initialCapacity)
	: device(device), graphicsQueue(graphicsQueue), commandPool(commandPool), allocator(allocator), capacity(initialCapacity), used(0)
{
	createBuffer(capacity,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		&_indexBuffer, &allocation,
		VMA_MEMORY_USAGE_GPU_ONLY);
}

buffers::indexBuffer::~indexBuffer()
{
	vmaDestroyBuffer(allocator, _indexBuffer, allocation);
}

void buffers::indexBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer* buffer, VmaAllocation* allocation, VmaMemoryUsage memoryUsage)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;

	if (vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, buffer, allocation, nullptr) != VK_SUCCESS) {
		LOG_CRITICAL("failed to create buffer with VMA");
	}
}

void buffers::indexBuffer::copyBufferRegion(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void buffers::indexBuffer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void buffers::indexBuffer::ensureCapacity(VkDeviceSize requiredSize)
{
	if (requiredSize > capacity) {
		VkDeviceSize newCapacity = std::max(capacity * 2, requiredSize);
		VkBuffer newBuffer;
		VmaAllocation newAllocation;
		createBuffer(newCapacity,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			&newBuffer, &newAllocation,
			VMA_MEMORY_USAGE_GPU_ONLY);

		copyBufferRegion(_indexBuffer, newBuffer, used, 0, 0);

		vmaDestroyBuffer(allocator, _indexBuffer, allocation);

		_indexBuffer = newBuffer;
		allocation = newAllocation;
		capacity = newCapacity;
	}
}

VkCommandBuffer buffers::indexBuffer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

size_t buffers::indexBuffer::appendIndices(const std::vector<uint32_t>& newIndices) {
	VkDeviceSize newDataSize = sizeof(uint32_t) * newIndices.size();
	ensureCapacity(used + newDataSize);

	VkBuffer stagingBuffer;
	VmaAllocation stagingAllocation;
	createBuffer(newDataSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		&stagingBuffer, &stagingAllocation,
		VMA_MEMORY_USAGE_CPU_ONLY);

	void* data;
	vmaMapMemory(allocator, stagingAllocation, &data);
	memcpy(data, newIndices.data(), static_cast<size_t>(newDataSize));
	vmaUnmapMemory(allocator, stagingAllocation);

	copyBufferRegion(stagingBuffer, _indexBuffer, newDataSize, 0, used);

	vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

	size_t offset = used;
	used += newDataSize;
	return offset;
}
