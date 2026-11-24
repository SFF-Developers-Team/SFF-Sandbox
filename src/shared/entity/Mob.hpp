#pragma once
#include <entity/Entity.hpp>

class Mob : public Entity {
protected:
    int m_health;
    float m_fallY;
    uint64_t m_lastHurtTime;

public:
    Mob(World& world);

    void onTick(World& world) override;

    virtual int getMaxHealth() { return 20; }
    int getHealth() { return m_health; }
    float getLastHurtTime() { return m_lastHurtTime; }

    DataStream serialize() override;
    bool deserialize(DataStream& stream) override;
};