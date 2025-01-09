#pragma once
#include <entity/SimplePlayer.hpp>
#include <PacketManager.hpp>
#include <GamePacket.hpp>
#include <Types.hpp>

#include <memory>

class Client : public PacketManager {
private:
    PlayerID m_id;
    bool m_loggedIn = false;
    bool m_disconnect = false;

public:
    Client(ENetPeer* peer);

    bool accept(Packet& packet);
    void update();
    void onPacketReceived(Packet& packet) override;
    void disconnect(DisconnectReasonID reason);

    void handle(Packet& packet) override;
    void handleBlock(Packet& packet);
    void handlePlayer(Packet& packet);
    void handleLoadChunk(Packet& packet);
    void handleLoadPlayer(Packet& packet);
    void handleBlockPlace(Packet& packet);
    void handleBlockDestroy(Packet& packet);

    auto const getPlayerID() {
        return m_id;
    }
};