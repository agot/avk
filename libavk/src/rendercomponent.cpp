#include "rendercomponent.h"

#include "entity.h"
#include "meshcomponent.h"

namespace avk {

void RenderComponent::onAdd(const Entity* parent) {
    assert(parent->hasComponent<MeshComponent>());
    _meshComponent = parent->getComponent<MeshComponent>();
}

void RenderComponent::draw() {
    if ( auto meshComponent = _meshComponent.lock() ) {
    }
}

} // namespace avk
