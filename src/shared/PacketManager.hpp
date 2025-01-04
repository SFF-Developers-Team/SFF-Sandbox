#pragma once
#include <vector>
#include <GamePacket.hpp>
#include <sockpp/connector.h>
#include <sockpp/acceptor.h>
#include <memory>
#include <map>
#include <mutex>

using QueueID = uint32_t;
using Header = SerializedObject::Header;

template<typename Socket>
class PacketManager {
private:
    int m_readBufSize;

protected:
    std::map<QueueID, std::shared_ptr<SerializedObject>> m_queue;
    QueueID m_lastId;
    Socket m_sock;
    std::mutex m_sendMutex;

public:
    PacketManager(Socket sock = Socket(), int readBufSize = 1024 * 64) : 
        m_sock(std::move(sock)), m_readBufSize(readBufSize), m_lastId(0) 
    {
        m_sock.read_timeout(std::chrono::seconds(10));
        m_sock.set_non_blocking(false);
        m_sock.set_option(SOL_SOCKET, SO_RCVBUF, &m_readBufSize, sizeof(m_readBufSize));
        m_sock.set_option(SOL_SOCKET, SO_SNDBUF, &m_readBufSize, sizeof(m_readBufSize));
    }

    ~PacketManager() {
        m_sock.shutdown();
    }

    bool send(GamePacket const& packet) {
        std::lock_guard lock(m_sendMutex);
        int written = 0;

        while(written < packet.size()) {
            auto res = m_sock.write(packet.data(), packet.size()).value();
            
            if(res == -1) return false;

            written = res;
        }

        return true;
    }

    GamePacket const recv() {
        auto bytes = ByteVector(m_readBufSize);
        auto res = m_sock.read(bytes.data(), bytes.size());
    
        if(res.value() > 0) {
            bytes.resize(res.value());

            return GamePacket(bytes);
        }

        return GamePacket(Header::NETWORK_ERROR, res.error_message());   
    }

    void addToQueue(std::shared_ptr<SerializedObject> classObj) {
        auto it = std::find_if(m_queue.begin(), m_queue.end(),
                            [&classObj](auto&& p) { return p.second == classObj; });
                
        if(it != m_queue.end()) return;

        m_queue.insert(std::make_pair(m_lastId, classObj));
        m_lastId++;
    }

    void sendQueue() {
        if(m_queue.size() > 1) {
            auto arr = CREATE_PACKET(Header::ARRAY, static_cast<uint16_t>(m_queue.size()));
            for(auto& [_, q] : m_queue) {
                if(q == nullptr || q->getI<Header>() == Header::ARRAY) continue;

                auto bytes = q->serialize();
                arr->add<uint16_t>(bytes.size());
                arr->add(bytes);
            }

            m_queue.clear();
            addToQueue(arr);
        }

        for(auto& [id, obj] : m_queue) {
            auto pak = GamePacket(Header::PACKET, id);
            auto pak2 = GamePacket(obj->serialize());
            auto hash = pak2.hash();
            auto size = pak2.size();
            pak.add<uint32_t>(hash);
            pak.add<uint32_t>(size);
            pak.add(pak2.bytes());

            send(pak);
        }

        // m_queue.clear();
    }

    virtual void handle(GamePacket& packet) {
        switch(packet.get<Header>()) {
            case Header::PACKET: handlePacket(packet); break;
            case Header::ARRAY: handleArray(packet); break;
            case Header::OK: handleOk(packet); break;
            default: break;
        }

        packet.reset();
    }

    void handleOk(GamePacket& packet) {
        auto id = packet.get<QueueID>();

        if(m_queue.contains(id)) m_queue.erase(id);
    }

    void handleArray(GamePacket& packet) {
        auto count = packet.get<uint16_t>();

        while(count-- > 0) {
            auto size = packet.get<uint16_t>();
            auto bytes = packet.getN(size);
            auto packet = GamePacket(bytes);
            
            handle(packet);
        }
    };

    void handlePacket(GamePacket& packet) {
        auto id = packet.get<QueueID>();
        auto hash = packet.get<uint32_t>();
        auto size = packet.get<uint32_t>();
        auto pak = GamePacket(packet.getN(size));

        logD("Packet came size: {} | hash: {:X} | real size: {} | real hash: {:X}", size, hash, pak.size(), pak.hash());

        if(pak.size() == size && pak.hash() == hash) {
            while(!send(GamePacket(Header::OK, id)));
            handle(pak);
        }
    }
};