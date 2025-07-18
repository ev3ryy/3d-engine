#ifndef MESH_RENDERER_COMPONENT_H
#define MESH_RENDERER_COMPONENT_H

#include "../component.h"

#include <mesh/mesh.h>
#include <material/material.h>

#include <memory>

class MeshRendererComponent : public component {
public:
	static const bool isUnique = true; // optional

	MeshRendererComponent(std::shared_ptr<Mesh> mesh, const std::string& materialID);
	~MeshRendererComponent() override = default;

	const std::shared_ptr<Mesh>& getMesh() const;
	const std::string& getMaterialID() const;
	void setMaterialId(const std::string& id);

private:
	std::shared_ptr<Mesh> mesh;
	std::string materialID;

};

#endif // MESH_RENDERER_COMPONENT_H