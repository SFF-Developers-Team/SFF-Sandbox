#pragma once
#include <GamePacket.hpp>
#include <queue>
#include <memory>
#include <variant>
#include <sockpp/tcp_socket.h>
#include <SimplePlayer.hpp>
#include <PacketManager.hpp>

class Client : public PacketManager<sockpp::tcp_socket> {
private:
    PlayerID m_id;
    bool m_loggedIn = false;
    bool m_shouldDisconnect = false;

public:
    Client(sockpp::tcp_socket sock);

    // Should be executed in separate thread!!
    bool accept();

    void inThread();
    void outThread();

    auto shouldDisconnect() { return m_shouldDisconnect; }
    auto getPlayerID() { return m_id; }

    void handle(GamePacket& packet) override;
    void handleBlock(GamePacket& packet);
    void handlePlayer(GamePacket& packet);
    void handleLoadPlayer(GamePacket& packet);
};