#ifndef RENDERER_FABRIC_H
#define RENDERER_FABRIC_H

#include <irenderer.h>
#include <shaders/shader_manager.h>

#include <vulkan/vulkan_pipeline.h>

namespace RendererFabric {
	IPipeline* createPipeline(API_TYPE api_type, ShaderManager* shaderManager) {
		switch (api_type)
		{
		case Vulkan:
			return new VulkanPipeline(shaderManager);
		case DX12:
			break;
		case OpenGL:
			break;
		default:
			break; return nullptr;
		}

		return nullptr;
	}
}

#endif // RENDERER_FABRIC_H