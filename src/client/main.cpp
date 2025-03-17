#include <Game.hpp>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments(argv + 1, argv + argc);
    Game::get()->init(arguments);

    return EXIT_SUCCESS;
}
