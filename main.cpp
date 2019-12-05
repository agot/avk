#include <avk.h>

#include <iostream>
#include <chrono>

class RootEntity : public avk::Entity {
public:
    RootEntity() {
        _spriteEntity = std::make_shared<avk::SpriteEntity>();
        addChild(_spriteEntity);
    }

    virtual ~RootEntity() {
    }

    virtual void update(const std::chrono::microseconds& deltaTime) override {
    }

private:
    std::shared_ptr<avk::SpriteEntity> _spriteEntity;

};

int main(int argc, char* argv[]) {
    boost::program_options::options_description options("option");
    options.add_options()
        ("threads", boost::program_options::value<uint32_t>(), "number of threads");
    boost::program_options::variables_map variables;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variables);
    }
    catch (const boost::program_options::error_with_option_name& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    uint32_t threads = 0;
    uint32_t maxFps = 60;
    boost::program_options::notify(variables);
    try {
        if (variables.count("threads") > 0) {
            threads = variables["threads"].as<uint32_t>();
        }
        if (variables.count("max-fps") > 0) {
            maxFps = variables["max-fps"].as<uint32_t>();
        }
    }
    catch (const boost::bad_any_cast& e) {
        std::cout << e.what() << std::endl;
    }

    avk::Avk avk({
        .threads = threads,
        .frameRate = maxFps
    });

    auto world = std::make_shared<avk::World>();
    auto entity = std::make_shared<RootEntity>();
    world->setRootEntity(entity);

    avk.setWorld(world);
    avk.run();

    return 0;
}
