#ifndef AVK_RENDER_COMPONENT_H_
#define AVK_RENDER_COMPONENT_H_

#include "component.h"

namespace avk {

class MeshComponent;
class RenderComponent : public Component<RenderComponent> {
public:
    RenderComponent() = default;
    virtual ~RenderComponent() = default;
    RenderComponent(const RenderComponent& world) = default;
    RenderComponent(RenderComponent&& world) = default;
    RenderComponent& operator=(const RenderComponent& world) = default;
    RenderComponent& operator=(RenderComponent&& world) = default;

    virtual void onAdd(const Entity* parent) override;

    virtual void draw() override;

private:
    std::weak_ptr<MeshComponent> _meshComponent;

};

} // namespace avk


#endif // #ifndef AVK_RENDER_COMPONENT_H_
