#pragma once

class Serializable;
class Packet;

typedef struct _ENetPeer ENetPeer;

enum Channel : unsigned char {
    EVERYTHING,
    TILES,
    NOTIFICATIONS,
    LAST_CHANNEL
};

class PacketManager {
protected:
    ENetPeer* m_peer;

public:
    PacketManager(ENetPeer* sock);
    ~PacketManager();

    bool sendObj(Serializable* obj, Channel channel = EVERYTHING, bool reliable = true);
    bool sendPacket(Packet const& packet, Channel channel = EVERYTHING, bool reliable = true);
    virtual void packetReceived(Packet& packet);

    virtual void handle(Packet& packet);
    void handleArray(Packet& packet);
};