#include "avk.h"

#include <chrono>
#include <iostream>

namespace avk {

Avk::Avk(const Config& config) : config(config) {
    window = std::make_unique<Window<Vulkan>>();
}

void Avk::setWorld(const std::shared_ptr<World>& world) {
    this->world = world;
}

void Avk::run() {
    using namespace std;
    const chrono::microseconds frameDuration(static_cast<int>(std::micro::den / config.frameRate));
    auto now = chrono::high_resolution_clock::now();
    auto prev = chrono::high_resolution_clock::now();
    while ( true ) {
        auto delta = chrono::duration_cast<chrono::microseconds>(now - prev);
        auto next = now + frameDuration;
        if ( world ) {
            world->update(delta);
        }
        window->update(delta);
        this_thread::sleep_until(next);
        prev = now;
        now = next;
    }
}

} // namespace avk
