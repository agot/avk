#ifndef AVK_COMPONENT_H_
#define AVK_COMPONENT_H_

#include <boost/type_index/ctti_type_index.hpp>

#include "componentinterface.h"
#include "crc32.hpp"

namespace avk {

template<typename T>
class Component : public ComponentInterface {
public:
    static constexpr uint32_t hash = crc32(boost::typeindex::ctti_type_index::type_id<T>().raw_name());

public:
    Component() = default;
    virtual ~Component() = default;
    Component(const Component& world) = default;
    Component(Component&& world) = default;
    Component<T>& operator=(const Component& world) = default;
    Component<T>& operator=(Component&& world) = default;

    size_t getHash() const override {
        return hash;
    }

    void onAdd(const Entity* parent) override {
        // nothing to do
    }

    void onRemove() override {
        // nothing to do
    }

    void update(const std::chrono::microseconds& deltaTime) override {
        // nothing to do
    }

    void postUpdate(const std::chrono::microseconds& deltaTime) override {
        // nothing to do
    }

    void draw() override {
        // nothing to do
    }

private:

};

} // namespace avk

#endif // #ifndef AVK_COMPONENT_H_
