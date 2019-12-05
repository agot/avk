#include "world.h"

#include <iostream>

#include "entity.h"

namespace avk {

void World::setRootEntity(const std::shared_ptr<Entity>& rootEntity) {
    _rootEntity = rootEntity;
}

void World::update(const std::chrono::microseconds& deltaTime) {
    if ( _rootEntity ) {
        _rootEntity->update(deltaTime);
        _rootEntity->updateComponents(deltaTime);
        _rootEntity->updateChildren(deltaTime);

        _rootEntity->postUpdate(deltaTime);
        _rootEntity->postUpdateComponents(deltaTime);
        _rootEntity->postUpdateChildren(deltaTime);

        _rootEntity->drawComponents();
        _rootEntity->drawChildren();
    }
}

}  // namespace avk
