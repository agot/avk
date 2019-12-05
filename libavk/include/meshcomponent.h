#ifndef AVK_MESH_COMPONENT_H_
#define AVK_MESH_COMPONENT_H_

#include "types.h"
#include "component.h"

namespace avk {

class MeshComponent : public Component<MeshComponent> {
public:
    MeshComponent(const std::vector<float>& verts, const std::vector<float>& norms, const std::vector<float>& uv);
    virtual ~MeshComponent() = default;
    MeshComponent(const MeshComponent& world) = default;
    MeshComponent(MeshComponent&& world) = default;
    MeshComponent& operator=(const MeshComponent& world) = default;
    MeshComponent& operator=(MeshComponent&& world) = default;

private:
    std::vector<float> _verts;
    std::vector<float> _norms;
    std::vector<float> _uv;

};

} // namespace avk

#endif // #ifndef AVK_MESH_COMPONENT_H_
