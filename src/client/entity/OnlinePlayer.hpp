#include <SimplePlayer.hpp>

class OnlinePlayer : public SimplePlayer {
public:
    OnlinePlayer(std::shared_ptr<World> world);
    void onTick() override;
};