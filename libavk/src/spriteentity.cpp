#include "spriteentity.h"

#include "meshcomponent.h"
#include "rendercomponent.h"

namespace avk {

SpriteEntity::SpriteEntity() {
    std::vector<float> verts = {
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        1.0, 1.0, 0.0
    };
    std::vector<float> norms = {
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0
    };
    std::vector<float> uv = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };
    addComponent<MeshComponent>(verts, norms, uv);
    addComponent<RenderComponent>();
}

} // namespace avk
