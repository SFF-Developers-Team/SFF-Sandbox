#pragma once
#include <deque>
#include <Packet.hpp>
#include <memory>
#include <map>
#include <mutex>
#include <enet.h>

using Header = SerializedObject::Header;

enum Channel : uint8_t {
    EVERYTHING,
    BLOCKS,
    NOTIFICATIONS,
    LAST_CHANNEL
};

class PacketManager {
protected:
    ENetPeer* m_peer;

public:
    PacketManager(ENetPeer* sock);
    ~PacketManager();

    bool sendObj(std::shared_ptr<SerializedObject> obj, Channel channel = EVERYTHING, bool reliable = true);
    bool sendPacket(Packet const& packet, Channel channel = EVERYTHING, bool reliable = true);
    virtual void packetReceived(Packet& packet);

    virtual void handle(Packet& packet);
    void handleArray(Packet& packet);
};