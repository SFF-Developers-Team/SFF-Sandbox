#include <Game.hpp>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
int main(int argc, char* argv[]) {
    std::vector<std::string> arguments(argv + 1, argv + argc);
    Game::get()->init(arguments);

    return 0;
}
