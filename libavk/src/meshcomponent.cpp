#include "meshcomponent.h"

namespace avk {

MeshComponent::MeshComponent(const std::vector<float>& verts, const std::vector<float>& norms, const std::vector<float>& uv) {
    _verts = verts;
    _norms = norms;
    _uv = uv;
}

} // namespace avk
