#include <Server.hpp>

int main() {
    auto server = Server::get();
    server->init();

    return 0;
}