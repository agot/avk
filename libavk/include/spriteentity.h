#ifndef AVK_SPRITE_ENTITY_
#define AVK_SPRITE_ENTITY_

#include "entity.h"

namespace avk {

class SpriteEntity : public Entity {
public:
    SpriteEntity();
    virtual ~SpriteEntity() = default;
    SpriteEntity(const SpriteEntity& world) = default;
    SpriteEntity(SpriteEntity&& world) = default;
    SpriteEntity& operator=(const SpriteEntity& world) = default;
    SpriteEntity& operator=(SpriteEntity&& world) = default;

private:

};

} // namespace avk

#endif // #ifndef AVK_SPRITE_ENTITY_
