#pragma once
#include <deque>
#include <GamePacket.hpp>
#include <memory>
#include <map>
#include <mutex>
#include <enet.h>

using QueueID = uint32_t;
using Header = SerializedObject::Header;

class PacketManager {
private:
    int m_readBufSize;

protected:
    std::deque<std::shared_ptr<SerializedObject>> m_queue;
    QueueID m_lastId;
    ENetPeer* m_peer;

public:
    PacketManager(ENetPeer* sock, int readBufSize = 1024 * 64);
    ~PacketManager();

    bool send(Packet& packet);
    virtual void onPacketReceived(Packet& packet);

    void addToQueue(std::shared_ptr<SerializedObject> classObj);
    void sendQueue();

    virtual void handle(Packet& packet);
    void handleArray(Packet& packet);
    // void handlePacket(Packet& packet);
};