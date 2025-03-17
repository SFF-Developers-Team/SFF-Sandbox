#pragma once
#include <inventory/InventoryItem.hpp>
#include <map>
#include <memory>
#include <mutex>
#include <Types.hpp>
#include <entity/Mob.hpp>
#include <inventory/Inventory.hpp>

class Player : public Mob, public Inventory {
protected:
    AnimationType m_animType = PLAYER_IDLE;
    uint8_t m_animFps = 10;
    uint8_t m_animFrame = 0;

    // multiplayer features
    PlayerID m_id;
    std::string m_username;
    std::mutex m_mutex;
    GameMode m_gamemode;

    bool m_fly;
    bool m_breakingBlock;

public:
    Player(std::shared_ptr<World> world);

    uint8_t animationClamp(uint8_t value, uint8_t min, uint8_t max) { return (value > max || value < min) ? min : value; }
    void setAnimation(AnimationType type);

    /// @brief For debug
    /// @return Animation name as string
    const char* getAnimationName();

    Vec2i static getAnimLimit(AnimationType type);

    DataStream serialize() override;
    bool deserialize(DataStream& bytes) override;

    void setUsername(std::string const& username) { m_username = username; }
    void setAnimCurrentFrame(uint8_t frame) { m_animFrame = frame; }
    void setID(PlayerID id) { m_id = id; }

    bool canDestroyBlock(World* world, BlockPosition target);
    bool canPlaceBlock(World* world, BlockPosition target);
    bool canAccessBlock(BlockPosition target);

    auto getAnimCurrentFrame() { return m_animFrame; }
    auto& getUsername() { return m_username; }
    auto getPlayerID() { return m_id; }

    GameMode getGamemode() { return m_gamemode; }
    void setGamemode(GameMode gamemode);

    void toggleFly();

    std::shared_ptr<InventoryItem> getItemHand() { return m_inventory[m_selected]; }

    void setBreakingBlock(bool flag) { m_breakingBlock = flag; }
    bool isBreakingBlock() { return m_breakingBlock; }
};