#pragma once
#include <Scene.hpp>
#include <string>
#include <vector>

class CreditsScene : public Scene {
private:
    struct Developer {
        std::string const texture;
        std::string const name;
        std::string const role;
    };

    // clang-format off
    std::vector<Developer> devs = {
        {"dogotrigger.png", "dogotrigger", "Programmer"},
        {"kolyah35.png", "Kolyah35", "Programmer"},
        {"invisedivine.png", "InviseDivine", "Programmer"},
        {"del.png", "del", "Artist"},
        {"e2e4.png", "e2e4", "Artist"}
    };
    // clang-format on

    Vec2f const picsize = {128.f, 128.f};
    int const yoffset = 30;
    int const textoffset = 10;
    int const textheight = 25;
    int const devsoffset = 30;

public:
    void draw() override;
    void update() override;
};