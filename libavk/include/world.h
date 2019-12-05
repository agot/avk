#ifndef AVK_WORLD_H_
#define AVK_WORLD_H_

#include <memory>
#include <chrono>

namespace avk {

class Entity;
class World {
public:
    World() = default;
    virtual ~World() = default;
    World(const World& world) = default;
    World(World&& world) = default;
    World& operator=(const World& world) = default;
    World& operator=(World&& world) = default;

    virtual void setRootEntity(const std::shared_ptr<Entity>& rootEntity);

    virtual void update(const std::chrono::microseconds& deltaTime);

private:
    std::shared_ptr<Entity> _rootEntity;

};

}  // namespace avk

#endif // #ifndef AVK_WORLD_H_
