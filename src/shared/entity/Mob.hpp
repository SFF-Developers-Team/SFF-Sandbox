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

    void onTick(World* world);

    virtual int getMaxHealth() { return 20; }
    int getHealth() { return m_health; }
    float getLastHurtTime() { return m_lastHurtTime; }

    DataStream serialize();
    bool deserialize(DataStream& stream);
};