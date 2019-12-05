#ifndef AVK_ENTITY_H_
#define AVK_ENTITY_H_

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>
#include <chrono>

#include "types.h"
#include "componentinterface.h"

namespace avk {

class Entity {
public:
    friend class World;

    Entity() = default;
    virtual ~Entity() = default;
    Entity(const Entity& world) = default;
    Entity(Entity&& world) = default;
    Entity& operator=(const Entity& world) = default;
    Entity& operator=(Entity&& world) = default;


    virtual void setTransform(const Transform& transform);

    virtual const Transform& getTransform() const;

    virtual void update(const std::chrono::microseconds& deltaTime) {
        // nothing to do
    }

    virtual void postUpdate(const std::chrono::microseconds& deltaTime) {
        // nothing to do
    }

    template<ComponentCompatible C, typename... Args>
    std::weak_ptr<C> addComponent(Args... args) {
        // check duplicate component
        assert(std::find(_componentHashes.begin(), _componentHashes.end(), C::hash) == _componentHashes.end());

        _componentHashes.push_back(C::hash);
        _components.push_back(std::make_shared<C>(args...));
        auto& component = _components.back();

        // call onAdd
        component->onAdd(this);

        return std::static_pointer_cast<C>(component);
    }

    template<ComponentCompatible C>
    void removeComponent() {
        auto itr = std::find(_componentHashes.begin(), _componentHashes.end(), C::hash);
        size_t index = std::distance(_componentHashes.begin(), itr);

        // check not found
        assert(index != _componentHashes.size());

        // call onRemove
        _components[index]->onRemove();

        std::erase(_componentHashes, C::hash);
        std::erase(_components, _components[index]);
    }

    template<ComponentCompatible C>
    std::weak_ptr<C> getComponent() const {
        auto itr = std::find(_componentHashes.begin(), _componentHashes.end(), C::hash);
        size_t index = std::distance(_componentHashes.begin(), itr);

        // check not found
        assert(index != _componentHashes.size());

        return std::static_pointer_cast<C>(_components[index]);
    }

    template<ComponentCompatible C>
    bool hasComponent() const {
        return std::find(_componentHashes.begin(), _componentHashes.end(), C::hash) != _componentHashes.end();
    }

    virtual void addChild(const std::shared_ptr<Entity>& child);

    virtual void removeChild(const std::shared_ptr<Entity>& child);

    const std::vector<std::shared_ptr<Entity>>& getChildren() const;

    void updateComponents(const std::chrono::microseconds& deltaTime);
    void postUpdateComponents(const std::chrono::microseconds& deltaTime);
    void drawComponents();

    void updateChildren(const std::chrono::microseconds& deltaTime);
    void postUpdateChildren(const std::chrono::microseconds& deltaTime);
    void drawChildren();

private:
    Transform _transform;
    std::vector<std::shared_ptr<Entity>> _children;
    std::vector<size_t> _componentHashes;
    std::vector<std::shared_ptr<ComponentInterface>> _components;
};

} // namespace avk

#endif // #ifndef AVK_ENTITY_H_
