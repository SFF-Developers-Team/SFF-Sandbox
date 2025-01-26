#pragma once
#include <ui/MenuBase.hpp>
#include <string>
#include <vector>

class CreditsScene : public MenuBase {
private:
    struct Developer {
        std::string const name;
        std::string const role;
    };

    // clang-format off
    std::vector<Developer> devs = {
        {"del", "Artist"},
        {"e2e4", "Artist"},
        {"InviseDivine", "Programmer"},
        {"Kolyah35", "Programmer"},
        {"dogotrigger", "Programmer"},
    };
    // clang-format on
    Vec2f const picsize = {128.f, 128.f};
    int const yoffset = 30;
    int const textoffset = 10;
    int const textheight = 25;
    int const devsoffset = 30;
public:
    CreditsScene();

    void draw() override;
};