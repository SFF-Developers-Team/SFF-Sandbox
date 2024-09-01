#pragma once
#include <GamePacket.hpp>
#include <queue>
#include <memory>
#include <sockpp/tcp_socket.h>
#include <PacketManager.hpp>
#include <SimplePlayer.hpp>

class Client {
private:
    std::unique_ptr<PacketManager<sockpp::tcp_socket>> m_pacman;
    std::vector<std::shared_ptr<GamePacket>> m_queue;
    std::vector<PlayerID> m_notifications;
    sockpp::tcp_socket m_sock;
    PlayerID m_id;
    bool m_loggedIn = false;
    bool m_shouldDisconnect = false;

public:
    Client(sockpp::tcp_socket sock);
    ~Client();

    // Should be executed in separate thread!!
    bool accept();

    void onTick();

    void addToQueue(std::shared_ptr<GamePacket> packet);
    void notify(PlayerID id);

    auto shouldDisconnect() { return m_shouldDisconnect; }
    auto getPlayerID() { return m_id; }
};