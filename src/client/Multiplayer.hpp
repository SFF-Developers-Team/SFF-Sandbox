#pragma once

#ifdef _WIN32
    #define NOGDI
    #define NOUSER
#endif

#include <PacketManager.hpp>
#include <sockpp/tcp_connector.h>
#include <GamePacket.hpp>
#include <Types.hpp>
#include <memory>

class Multiplayer {
private:
    PacketManager<sockpp::tcp_connector>* m_pacman;
    sockpp::tcp_connector m_connector;
    std::vector<std::shared_ptr<SerializedObject>> m_queue;
    bool m_worldLoaded = false;

public:
    ~Multiplayer();

    bool connect(std::string const& host, in_port_t port);
    void onTick();

    void addToQueue(std::shared_ptr<SerializedObject> classObj);
    void onBlockChanged(Vec2i pos, uint8_t layer);
};