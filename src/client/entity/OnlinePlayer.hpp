#pragma once
#include <entity/ClientPlayer.hpp>
#include <array>

struct OnlinePosition {
    float x;
    float y;
    float time;
};

class OnlinePlayer : public ClientPlayer {
protected:
    std::array<OnlinePosition, 2> m_positions;
    int m_index;

public:
    OnlinePlayer(World& world);

    void update();
    bool deserialize(DataStream& bytes);
};