#pragma once
#include <thread>
#include <memory>
#include <map>
#include <entity/SimplePlayer.hpp>
#include <Packet.hpp>
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

    std::shared_ptr<World> m_world;
    std::shared_ptr<Timer> m_timer;

    std::map<uint32_t, std::shared_ptr<Client>> m_clients;

public:
    static Server* get() {
        static auto server = new Server();
        return server;
    }

    void displayList();
    void displayHelp();
    void destroy();
    void inputThread();

    void init();
    void update();
    void onTick();

    void broadcast(std::shared_ptr<SerializedObject> obj, Channel channel = EVERYTHING, bool reliable = true);
    void broadcast(Packet const& packet, Channel channel = EVERYTHING, bool reliable = true);
    void broadcastExcept(PlayerID pid, std::shared_ptr<SerializedObject> obj, Channel channel = EVERYTHING, bool reliable = true);
    void broadcastExcept(PlayerID pid, Packet const& packet, Channel channel = EVERYTHING, bool reliable = true);
    void send(PlayerID pid, std::shared_ptr<SerializedObject> obj, Channel channel = EVERYTHING, bool reliable = true);
    void send(PlayerID pid, Packet const& packet, Channel channel = EVERYTHING, bool reliable = true);

    PlayerID joinPlayer(std::string const& username);
    void disconnectPlayer(std::shared_ptr<Client> client, DisconnectReasonID reason);

    std::string const getDisconnectReasonByID(DisconnectReasonID id);

    auto getWorld() { return m_world; }
    auto getTimer() { return m_timer; }
};