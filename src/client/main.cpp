#ifdef _WIN32
    #define NOGDI
    #define NOUSER
#endif

#include <Game.hpp>

int main(int argc, char* argv[]) {
#ifdef _WIN32
    setlocale(LOCALE_ALL, "ru");
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::vector<std::string> arguments(argv + 1, argv + argc);

    Game::get()->init(arguments);

    return 0;
}
