#include <Server.hpp>

#include <enet.h>

int main() {
    if(enet_initialize() != 0) {
        logE("An error occurred while initializing ENet.");
        return EXIT_FAILURE;
    }

    auto server = Server::get();
    server->init();

    return 0;
}