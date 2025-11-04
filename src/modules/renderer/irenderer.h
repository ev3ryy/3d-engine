#ifndef I_RENDERER_H
#define I_RENDERER_H

#include <material/material.h>

#include <renderer_data.h>

class IInstance {
public:
	virtual ~IInstance() = default;
	virtual void waitIdle() = 0;
};

class IDevice {
public:
	virtual ~IDevice() {};
	virtual void waitIdle() = 0;
	virtual bool IsValid() = 0;
};

class IRenderer {
public:
	virtual ~IRenderer() {};

	virtual bool IsValid() = 0;

	// initializing
	virtual void init() = 0;
	virtual void cleanup() = 0;

	// draw frame
	virtual FrameRenderStatus beginFrame() = 0;
	virtual void drawFrame(RenderFrameData& frameData) = 0;
	virtual FrameRenderStatus endFrame() = 0;

	// window
	virtual void notifyWindowResized() = 0;

	// etc
	virtual MaterialInstance* getOrCreateMaterialInstance(Material& material) = 0;

	virtual IInstance* getInstance() = 0;
	virtual IDevice* getDevice() = 0;
};

#endif // I_RENDERER_H