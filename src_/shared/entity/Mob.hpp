#pragma once
#include <entity/Entity.hpp>

class Mob : public Entity {
protected:
    int m_health;
    uint64_t m_lastHurtTime;
    float m_fallY;

    bool m_prevOnGround;

public:
    Mob(std::shared_ptr<World> world);

    void onTick();

    virtual int getMaxHealth() { return 20; }
    int getHealth() { return m_health; }
    float getLastHeartTime() { return m_lastHurtTime; }

    ByteVector serialize();
    size_t deserialize(ByteVector const& bytes);
};