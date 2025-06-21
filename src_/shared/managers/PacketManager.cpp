#include <managers/PacketManager.hpp>

PacketManager::PacketManager(ENetPeer* peer) : m_peer(peer) {}

PacketManager::~PacketManager() {
    enet_peer_disconnect(m_peer, 0);
}

bool PacketManager::sendObj(std::shared_ptr<SerializedObject> obj, Channel channel, bool reliable) {
    auto bytes = obj->serialize();
    // Uhm object allocated vector with bytes
    // Then enet copy vector to internal queue
    // Then vector is destroyed
    auto packet = enet_packet_create(bytes.data(), bytes.size(), (reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT));

    return enet_peer_send(m_peer, channel, packet) == 0;
}

bool PacketManager::sendPacket(Packet const& packet, Channel channel, bool reliable) {
    auto enetPacket = enet_packet_create(packet.data(), packet.size(), (reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT));

    return enet_peer_send(m_peer, channel, enetPacket) == 0;
}

void PacketManager::packetReceived(Packet& packet) {
    handle(packet);
}

void PacketManager::handle(Packet& packet) {
    if(packet.get<Header>() == Header::ARRAY) {
        handleArray(packet);
    }

    packet.reset();
}

void PacketManager::handleArray(Packet& packet) {
    auto count = packet.get<uint16_t>();

    while(count-- > 0) {
        auto size = packet.get<uint16_t>();
        auto pak = Packet(packet.getN(size));
        
        handle(pak);
    }
}