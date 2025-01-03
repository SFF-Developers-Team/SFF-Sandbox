#pragma once
#include <vector>
#include <GamePacket.hpp>
#include <sockpp/connector.h>
#include <sockpp/acceptor.h>
#include <memory>
#include <map>

using QueueID = uint32_t;
using Header = SerializedObject::Header;

template<typename Socket>
class PacketManager {
private:
    size_t m_readBufSize;

protected:
    std::map<QueueID, std::shared_ptr<SerializedObject>> m_queue;
    QueueID m_lastId;
    Socket m_sock;

public:
    PacketManager(Socket sock = Socket(), size_t readBufSize = 1024 * 64) : 
        m_sock(std::move(sock)), m_readBufSize(readBufSize), m_lastId(0) 
    {
        m_sock.read_timeout(std::chrono::seconds(3));
        m_sock.set_non_blocking(false);
    }

    ~PacketManager() {
        m_sock.shutdown();
    }

    bool send(GamePacket const& packet) {
        int written = 0;
        // uint16_t size = packet.size();
        // auto pak = GamePacket(size);
        // pak.resize(size + 2);
        // std::copy(packet.begin(), packet.end(), pak.data() + 2);

        while(written < packet.size()) {
            auto res = m_sock.write(packet.data() + written, packet.size() - written).value();
            if(res < 0) return false;

            written += res;
        }

        return true;
    }

    GamePacket const recv() {
        auto bytes = ByteVector(m_readBufSize);

        auto read = m_sock.read(bytes.data(), bytes.size());
        
        if(read.value() > 0) {
            bytes.resize(read.value());

            return GamePacket(bytes);
        }
    
        return GamePacket(Header::NETWORK_ERROR, read.error_message());
        
        // uint32_t read = 0;
        // uint16_t size = 0;

        // while(true) {
        //     auto res = m_sock.read(bytes.data() + read, bytes.size() - read);
        //     read += res.value();
            
        //     if(!size && read >= sizeof(uint16_t)) {
        //         size = *(uint16_t*)bytes.data();
        //         bytes.erase(bytes.begin(), bytes.begin() + 2);
        //         read -= sizeof(uint16_t);
        //     };

        //     if(res.is_error() || res.value() < 0) {
        //         return GamePacket(Header::NETWORK_ERROR, res.error_message());
        //     }

        //     if(read >= size && size > 0) {
        //         bytes.resize(size);

        //         return GamePacket(bytes);
        //     }
        // }
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
                if(q == nullptr) continue;

                auto bytes = q->serialize();
                arr->add<uint16_t>(bytes.size());
                arr->add(bytes);
            }

            m_queue.clear();
            addToQueue(arr);
        }

        if(!m_queue.empty()) {
            auto beg = m_queue.begin();
            auto pak = GamePacket(Header::PACKET, beg->first);
            auto pak2 = GamePacket(beg->second->serialize());
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
            handle(pak);
            send(GamePacket(Header::OK, id));
        }
    }
};