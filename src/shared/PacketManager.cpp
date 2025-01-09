#include <PacketManager.hpp>

PacketManager::PacketManager(ENetPeer* sock, int readBufSize) : 
        m_peer(sock), m_readBufSize(readBufSize), m_lastId(0) 
{
    // enet_peer_timeout(m_peer, 10, 5, 20);
    // m_sock.set_option(SOL_SOCKET, SO_RCVBUF, &m_readBufSize, sizeof(m_readBufSize));
    // m_sock.set_option(SOL_SOCKET, SO_SNDBUF, &m_readBufSize, sizeof(m_readBufSize));
}

PacketManager::~PacketManager() {
    enet_peer_disconnect(m_peer, 0);
}

bool PacketManager::send(Packet& packet) {
    auto bytes = packet.bytes();
    auto pak = enet_packet_create(bytes.data(), bytes.size(), ENET_PACKET_FLAG_RELIABLE);

    return enet_peer_send(m_peer, 0, pak) == 0;
}

void PacketManager::onPacketReceived(Packet& packet) {
    handle(packet);
}


void PacketManager::addToQueue(std::shared_ptr<SerializedObject> classObj) {
    auto it = std::find_if(m_queue.begin(), m_queue.end(),
                        [&classObj](auto& q) { return q == classObj; });
            
    if(it != m_queue.end()) return;

    m_queue.push_back(classObj);
}

void PacketManager::sendQueue() {
    if(m_queue.size() > 1) {
        auto arr = CREATE_PACKET(Header::ARRAY, static_cast<uint16_t>(m_queue.size()));
        for(auto& q : m_queue) {
            q->reset();
            if(q->getI<Header>() != Header::ARRAY) {
                auto bytes = q->serialize();
                arr->add<uint16_t>(bytes.size());
                arr->add(bytes);
            }
        }
        m_queue.clear();
        addToQueue(arr);
    }

    for(auto& obj : m_queue) {
        // auto pak = GamePacket(Header::PACKET, id);
        auto pak2 = Packet(obj->serialize());
        // auto hash = pak2.hash();
        // auto size = pak2.size();
        // pak.add<uint32_t>(hash);
        // pak.add<uint32_t>(size);
        // pak.add(pak2.bytes());
        send(pak2);
    }
    m_queue.clear();
}

void PacketManager::handle(Packet& packet) {
    switch(packet.get<Header>()) {
        // case Header::PACKET: handlePacket(packet); break;
        case Header::ARRAY: handleArray(packet); break;
        default: break;
    }

    packet.reset();
}

void PacketManager::handleArray(Packet& packet) {
    auto count = packet.get<uint16_t>();

    while(count-- > 0) {
        auto size = packet.get<uint16_t>();
        auto bytes = packet.getN(size);
        auto packet = Packet(bytes);
        
        handle(packet);
    }
};

// void PacketManager::handlePacket(Packet& packet) {
//     auto id = packet.get<QueueID>();
//     auto hash = packet.get<uint32_t>();
//     auto size = packet.get<uint32_t>();
//     auto pak = Packet(packet.getN(size));

//     logD("Packet came size: {} | hash: {:X} | real size: {} | real hash: {:X}", size, hash, pak.size(), pak.hash());
    
//     if(pak.size() == size && pak.hash() == hash) {
//         // send(GamePacket(Header::OK, id));
//         handle(pak);
//     }
// }