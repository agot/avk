#ifndef AVK_AVK_H_
#define AVK_AVK_H_

#include <thread>

#include <boost/program_options.hpp>

#include "vulkan.h"
#include "window.h"
#include "world.h"
#include "entity.h"
#include "spriteentity.h"
#include "meshcomponent.h"

namespace avk {

class Avk {
public:
    struct Config {
        uint32_t threads;
        uint32_t frameRate;
    };

public:
    Avk(const Config& config);
    virtual ~Avk() = default;
    void setWorld(const std::shared_ptr<World>& world);
    void run();

private:
    Config config;
    std::unique_ptr<Window<Vulkan>> window;
    std::shared_ptr<World> world;

};

}

#endif // #ifndef AVK_AVK_H_
