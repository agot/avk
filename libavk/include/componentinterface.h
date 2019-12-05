#ifndef AVK_COMPONENT_INTERFACE_H_
#define AVK_COMPONENT_INTERFACE_H_

#include <memory>
#include <chrono>

namespace avk {

class Entity;
class ComponentInterface {
public:
    virtual size_t getHash() const = 0;
    virtual void onAdd(const Entity* parent) = 0;
    virtual void onRemove() = 0;
    virtual void update(const std::chrono::microseconds& deltaTime) = 0;
    virtual void postUpdate(const std::chrono::microseconds& deltaTime) = 0;
    virtual void draw() = 0;
};

template<typename T>
concept ComponentCompatible = std::is_base_of<ComponentInterface, T>::value;

} // namespace avk

#endif // #ifndef AVK_COMPONENT_INTERFACE_H_
