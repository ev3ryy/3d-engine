#include "mesh_renderer_component.h"

MeshRendererComponent::MeshRendererComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
    mesh(mesh), material(material)
{
}

const std::shared_ptr<Mesh>& MeshRendererComponent::getMesh() const
{
    return mesh;
}

const std::shared_ptr<Material>& MeshRendererComponent::getMaterial() const
{
    return material;
}
