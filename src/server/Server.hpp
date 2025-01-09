#pragma once
#include <thread>
#include <memory>
#include <map>
#include <entity/SimplePlayer.hpp>
#include <GamePacket.hpp>
#include <toml.hpp>
#include <Client.hpp>
#include <enet.h>
#include <Types.hpp>

class World;
class Timer;

class Server {
private:
    ENetHost* m_server;
    toml::v3::table config;
    std::mutex m_acceptLock;

    std::shared_ptr<World> m_world;
    std::shared_ptr<Timer> m_timer;

    std::map<uint32_t, std::shared_ptr<Client>> m_clients;

public:
    static Server* get() {
        static auto server = new Server();
        return server;
    }

    void destroy();
    void inputThread();

    void init();
    void update();

    void addToQueueAll(std::shared_ptr<SerializedObject> packet);
    void addToQueue(PlayerID id, std::shared_ptr<SerializedObject> packet);
    void addToQueueExcept(PlayerID id, std::shared_ptr<SerializedObject> packet);

    PlayerID joinPlayer(std::string const& username);
    void disconnectPlayer(std::shared_ptr<Client> client, DisconnectReasonID reason);

    std::string const getDisconnectReasonByID(DisconnectReasonID id);

    auto getWorld() { return m_world; }
    auto getTimer() { return m_timer; }
    auto& getAcceptLock() { return m_acceptLock; }
};