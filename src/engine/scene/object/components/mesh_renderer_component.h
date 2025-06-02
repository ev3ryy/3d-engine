#ifndef MESH_RENDERER_COMPONENT_H
#define MESH_RENDERER_COMPONENT_H

#include "../component.h"

#include <mesh/mesh.h>
#include <material/material.h>

#include <memory>

class MeshRendererComponent : public component {
public:
	static const bool isUnique = false; // optional

	MeshRendererComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
	~MeshRendererComponent() override = default;

	const std::shared_ptr<Mesh>& getMesh() const;
	const std::shared_ptr<Material>& getMaterial() const;

private:
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

};

#endif // MESH_RENDERER_COMPONENT_H