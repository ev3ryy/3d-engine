#include "mesh_renderer_component.h"

MeshRendererComponent::MeshRendererComponent(std::shared_ptr<Mesh> mesh, const std::string& materialID) :
    mesh(mesh), materialID(materialID)
{
}

const std::shared_ptr<Mesh>& MeshRendererComponent::getMesh() const
{
    return mesh;
}

const std::string& MeshRendererComponent::getMaterialID() const
{
    return materialID;
}

void MeshRendererComponent::setMaterialId(const std::string& id)
{
    materialID = id;
}
