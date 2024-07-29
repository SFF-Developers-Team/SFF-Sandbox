#pragma once
#include <thread>
#include <mutex>
#include <map>
#include <sockpp/tcp_acceptor.h>
#include <SimplePlayer.hpp>
#include <GamePacket.hpp>

class World;

class Server {
private:
    sockpp::tcp_acceptor m_acceptor;
    World* m_world;

    std::map<PlayerID, std::vector<GamePacket*>> m_clientQueue;
    
public:
    void acceptThread(sockpp::tcp_socket sock);
    void init();
    void loop();
    void destroy();

    void addToQueueAll(GamePacket* packet);
    void addToQueue(PlayerID id, GamePacket* packet);
    void addToQueueExcept(PlayerID id, GamePacket* packet);

    GamePacket* read(sockpp::tcp_socket& sock, ByteVector& buf);
    GamePacket* read(sockpp::tcp_socket& sock, size_t n);
    GamePacket* read(sockpp::tcp_socket& sock);

    bool send(sockpp::tcp_socket& sock, GamePacket* packet);

    PlayerID joinPlayer(std::string const& username);
    void disconnectPlayer(PlayerID id);
    void closeSocket(sockpp::tcp_socket sock);
};