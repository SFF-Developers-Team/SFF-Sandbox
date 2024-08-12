#pragma once
#include <thread>
#include <memory>
#include <map>
#include <sockpp/tcp_acceptor.h>
#include <SimplePlayer.hpp>
#include <GamePacket.hpp>
#include <toml.hpp>

class World;

class Server {
private:
    sockpp::tcp_acceptor m_acceptor;
    World* m_world;
    toml::v3::table config;

    std::map<PlayerID, std::vector<std::shared_ptr<GamePacket>>> m_clientQueue;
    
public:
    static Server* get() {
        static auto server = new Server();
        return server;
    }

    void destroy();

    void acceptThread(sockpp::tcp_socket sock);
    void inputThread();
    void init();
    void loop();

    void addToQueueAll(std::shared_ptr<GamePacket> packet);
    void addToQueue(PlayerID id, std::shared_ptr<GamePacket> packet);
    void addToQueueExcept(PlayerID id, std::shared_ptr<GamePacket> packet);

    std::shared_ptr<GamePacket> read(sockpp::tcp_socket& sock, ByteVector& buf);
    std::shared_ptr<GamePacket> read(sockpp::tcp_socket& sock, size_t n);
    std::shared_ptr<GamePacket> read(sockpp::tcp_socket& sock);

    bool send(sockpp::tcp_socket& sock, std::shared_ptr<GamePacket> packet);

    PlayerID joinPlayer(std::string const& username);
    void disconnectPlayer(PlayerID id);
    void closeSocket(sockpp::tcp_socket sock);

    auto getWorld() { return m_world; }
};

void destroy();