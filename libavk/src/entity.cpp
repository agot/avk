#include "entity.h"

namespace avk {

void Entity::setTransform(const Transform& transform) {
    _transform = transform;
}

const Transform& Entity::getTransform() const {
    return _transform;
}

void Entity::updateComponents(const std::chrono::microseconds& deltaTime) {
    for ( auto& component : _components ) {
        component->update(deltaTime);
    }
}

void Entity::postUpdateComponents(const std::chrono::microseconds& deltaTime) {
    for ( auto& component : _components ) {
        component->postUpdate(deltaTime);
    }
}

void Entity::drawComponents() {
    for ( auto& component : _components ) {
        component->draw();
    }
}

void Entity::updateChildren(const std::chrono::microseconds& deltaTime) {
    for ( auto& child : _children ) {
        child->updateChildren(deltaTime);
    }
}

void Entity::postUpdateChildren(const std::chrono::microseconds& deltaTime) {
    for ( auto& child : _children ) {
        child->postUpdateChildren(deltaTime);
    }
}

void Entity::drawChildren() {
    for ( auto& child : _children ) {
        child->drawChildren();
    }
}

void Entity::addChild(const std::shared_ptr<Entity>& child) {
    _children.push_back(child);
}

void Entity::removeChild(const std::shared_ptr<Entity>& child) {
    std::erase(_children, child);
}

const std::vector<std::shared_ptr<Entity>>& Entity::getChildren() const {
    return _children;
}

} // namespace avk
