#ifndef RENDERER_FABRIC_H
#define RENDERER_FABRIC_H

#include <irenderer.h>

#include <vulkan/pipeline.h>

enum API_TYPE {
	Vulkan = 0,
	DX12 = 1,
	OpenGL = 2,
};

namespace RendererFabric {
	IRenderer* createPipeline(API_TYPE api_type) {
		switch (api_type)
		{
		case Vulkan:
			return new pipeline();
		case DX12:
			break;
		case OpenGL:
			break;
		default:
			break; nullptr;
		}
	}
}

#endif // RENDERER_FABRIC_H