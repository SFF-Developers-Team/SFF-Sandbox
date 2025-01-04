#pragma once
#include <GamePacket.hpp>
#include <queue>
#include <memory>
#include <string>
#include <variant>
#include <sockpp/tcp_socket.h>
#include <SimplePlayer.hpp>
#include <PacketManager.hpp>

class Client : public PacketManager<sockpp::tcp_socket> {
private:
    std::string m_lastError;
    PlayerID m_id;
    bool m_loggedIn = false;
    bool m_shouldDisconnect = false;

public:
    Client(sockpp::tcp_socket sock);

    // Should be executed in separate thread!!
    bool accept();

    void inThread();
    void outThread();

    auto const shouldDisconnect() { return m_shouldDisconnect; }
    auto const getPlayerID() { return m_id; }
    auto const& getLastError() { return m_lastError; }

    void handle(GamePacket& packet) override;
    void handleBlock(GamePacket& packet);
    void handleError(GamePacket& packet);
    void handlePlayer(GamePacket& packet);
    void handleLoadChunk(GamePacket& packet);
    void handleLoadPlayer(GamePacket& packet);
};