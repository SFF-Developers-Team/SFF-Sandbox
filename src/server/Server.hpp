#pragma once
#include <thread>
#include <memory>
#include <map>
#include <sockpp/tcp_acceptor.h>
#include <SimplePlayer.hpp>
#include <GamePacket.hpp>
#include <toml.hpp>
#include <Client.hpp>

class World;
class Timer;

class Server {
private:
    sockpp::tcp_acceptor m_acceptor;
    toml::v3::table config;
    std::mutex m_acceptLock;

    std::shared_ptr<World> m_world;
    std::shared_ptr<Timer> m_timer;

    std::vector<std::unique_ptr<Client>> m_clients;

public:
    static Server* get() {
        static auto server = new Server();
        return server;
    }

    void destroy();

    void onTick();
    void acceptThread(sockpp::tcp_socket sock);
    void inputThread();

    void init();
    void loop();

    void addToQueueAll(std::shared_ptr<SerializedObject> packet);
    void addToQueue(PlayerID id, std::shared_ptr<SerializedObject> packet);
    void addToQueueExcept(PlayerID id, std::shared_ptr<SerializedObject> packet);

    PlayerID joinPlayer(std::string const& username);
    void disconnectPlayer(PlayerID id);

    auto getWorld() { return m_world; }
    auto getTimer() { return m_timer; }
    auto& getAcceptLock() { return m_acceptLock; }
};